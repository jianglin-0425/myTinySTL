`deque` 里的 `map_`（在 GCC 源码中常命名为 `_M_map`）虽然名字里有 “map”，但它和 `std::map` 容器**毫无关系**。它的本质是一个**指针数组（或二级指针）**，作用是充当 `deque` 的“中控器”，用来管理那些分散的内存块。

### 为什么需要这个 `map_`？

这源于 `deque` 的设计目标：它需要像 `vector` 一样支持**随机访问**（`[]` 操作），又需要像 `list` 一样在**两端高效插入删除**。为了兼顾两者，`deque` 采用**分段连续空间**，而不是一整块连续内存。

1.  **数据分段存储**：`deque` 的元素实际存放在多个固定大小的**缓冲区**里。这些缓冲区在物理内存上通常是不连续的。
2.  **`map_` 负责索引**：既然数据分散，就需要一个“目录”来记录每个缓冲区在哪里。`map_` 就是这样一个数组，它的每一个元素都是一个指针，指向一个具体的缓冲区。
3.  **实现随机访问**：当你要访问 `deque` 的第 `n` 个元素时，迭代器不能简单地做指针加法（像 `vector` 那样）。它必须先通过 `map_` 找到第 `n` 个元素所在的缓冲区指针，再在缓冲区内部偏移，从而定位到元素。`map_` 的存在让这种“先查目录、再找内容”的机制得以实现。

### 源码中的样子

在 SGI STL（以及 GCC 的实现）中，`deque` 的关键成员通常包括：

*   `_M_map`：类型通常是 `T**`（指向指针的指针），这就是你问的 `map_`。它指向那块连续的中控器空间。
*   `_M_map_size`：记录这个中控器数组当前有多大，能容纳多少个缓冲区指针。
*   `_M_start` 和 `_M_finish`：两个迭代器，分别指向第一个元素和最后一个元素的下一个位置。它们内部也包含指向 `map_` 中某个位置的指针，以便在缓冲区之间移动。

简单说，**`map_` 是 `deque` 实现“分段连续”语义的核心枢纽**。没有它，`deque` 就无法在两端扩展的同时，还维持表面上的连续访问能力。


这段代码是 **SGI STL（以及侯捷《STL源码剖析》配套的 mystl）中 `deque` 的初始化函数**，非常经典。它体现了 deque 最核心的设计思想：**用一块中控 map 管理多段固定大小的缓冲区，并在中央预留空间以便向两端扩展**。

下面逐段拆解。

## 一、整体设计回顾

`deque` 的物理结构不是一段连续内存，而是：

```
map_ (指针数组/中控器)
  ↓
[ ptr ][ ptr ][ ptr ][ ptr ][ ptr ][ ... ]
         ↓            ↓
      缓冲区        缓冲区   (每块 buffer_size 个元素)
```

- `map_`：`T**`，指向指针数组，每个元素指向一块缓冲区
- `buffer_size`：每块缓冲区能容纳的元素个数
- `map_size_`：map 数组能容纳的指针个数
- `begin_` / `end_`：迭代器，分别指向首元素和尾后位置
- 迭代器内部有 `first`（缓冲区头）、`last`（缓冲区尾）、`cur`（当前）、`node`（在 map 中的位置）

关键点：**元素在缓冲区内部连续，跨缓冲区不连续**。所以 deque 是"分段连续"的。

## 二、逐行分析

### 1. 计算需要的缓冲区个数

```cpp
const size_type nNode = nElem / buffer_size + 1;
```

- `nElem` 是要初始化的元素个数
- `buffer_size` 每块缓冲区容量
- `nNode` 需要多少块缓冲区

注意这里用的是 `+1` 而不是向上取整 `(nElem + buffer_size - 1) / buffer_size`。因为**即使 nElem 是 buffer_size 的整数倍，也可能需要多一块来放"尾后"位置**。实际上这是简化处理，保证至少有一块且不越界。

### 2. 决定 map 数组大小

```cpp
map_size_ = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
```

- `DEQUE_MAP_INIT_SIZE` 通常为 8，是 map 的初始最小容量
- `nNode + 2`：在需要的缓冲区之外，**前后各预留 1 块空间**，方便后续 `push_front`/`push_back` 时无需立即扩充 map
- 取两者最大值

这就是 deque "**预留中央区域**" 设计的体现——让 map 中间放数据，两端留空。

### 3. 分配 map 数组（第一层 try）

```cpp
try {
    map_ = create_map(map_size_);
} catch (...) {
    map_ = nullptr;
    map_size_ = 0;
    throw;
}
```

`create_map` 分配 `map_size_` 个 `T*` 的指针数组。失败时清理状态并重新抛出，**保证异常安全（不泄漏、对象状态一致）**。

### 4. 计算数据区在 map 中的位置

```cpp
map_pointer nstart  = map_ + (map_size_ - nNode) / 2;
map_pointer nfinish = nstart + nNode - 1;
```

- 把 `nNode` 块数据区放在 map 的**正中央**
- `(map_size_ - nNode) / 2` 是左侧预留的空槽数
- `nstart` 是第一块数据缓冲区在 map 中的指针位置
- `nfinish` 是最后一块数据缓冲区的指针位置（`nstart + nNode - 1`）

**为什么放中央？** 因为 deque 要支持两端高效插入。如果数据靠左，`push_front` 时左侧没空间；靠右则 `push_back` 受限。放中央使两端各有约一半空槽，均摊扩展成本低。

### 5. 分配缓冲区（第二层 try）

```cpp
try {
    create_buffer(nstart, nfinish);
} catch (...) {
    map_allocator::deallocate(map_, map_size_);
    map_ = nullptr;
    map_size_ = 0;
    throw;
}
```

`create_buffer` 为 `[nstart, nfinish]` 范围内的每个槽分配一块 `buffer_size` 大小的缓冲区。失败时**回滚已分配的 map 数组**，再次保证异常安全。这种"嵌套 try + 逐层释放"是 STL 异常安全的标准写法。

### 6. 设置迭代器

```cpp
begin_.set_node(nstart);
end_.set_node(nfinish);
begin_.cur = begin_.first;
end_.cur   = end_.first + (nElem % buffer_size);
```

- `begin_` 指向第一块缓冲区的起始位置：`cur = first`
- `end_` 指向最后一块缓冲区内的偏移位置：
    - `nElem % buffer_size` 是最后一个缓冲区中已用元素数
    - `end_.cur = end_.first + (nElem % buffer_size)` 指向**尾后位置**

**这里有个细节需要注意**：当 `nElem % buffer_size == 0` 且 `nElem > 0` 时，`end_.cur == end_.first`，即 end 落在最后一块缓冲区的开头。这在 deque 里是合法状态（尾后位置可以在缓冲区头），但要配合 `nfinish` 的计算一起理解。实际上 SGI 原始实现中 `nNode` 的算法会保证这种情况 end 落在正确的缓冲区。

## 三、图示

假设 `nElem = 20`，`buffer_size = 8`，`DEQUE_MAP_INIT_SIZE = 8`：

- `nNode = 20/8 + 1 = 3`
- `map_size_ = max(8, 3+2) = 8`
- 数据区放中央：`nstart = map_ + (8-3)/2 = map_ + 2`，占 map_[2]、map_[3]、map_[4]
- 左侧 map_[0]、map_[1] 空闲，右侧 map_[5]~map_[7] 空闲
- `begin_.cur = buffer[2].first`
- `end_.cur = buffer[4].first + (20 % 8) = buffer[4].first + 4`

```
map_: [ ][ ][B0][B1][B2][ ][ ][ ]
              ↑         ↑
           begin_     end_(cur=+4)
```

## 四、这段代码体现的工程要点

1. **异常安全**：两层 try-catch，失败时逐层释放已获资源，对象状态归零，不泄漏、不留下半初始化状态。
2. **预留中央**：`(map_size_ - nNode) / 2` 让数据居中，为两端扩展留余地。
3. **多余预留**：`nNode + 2` 和 `max(DEQUE_MAP_INIT_SIZE, ...)` 减少后续 reallocate map 的频率。
4. **迭代器状态精确设置**：`begin_` 和 `end_` 不仅设 node，还要设 `cur`，尤其 end 的偏移处理是 deque 正确性的关键。

## 五、可以进一步思考的点

- 当 `nElem` 很大时 `nNode` 可能撑爆 `map_size_`，`create_map` 需要能处理大数组。
- `end_.cur == end_.first` 的边界情况在后续 `push_back` 中如何处理（会触发 `reserve_map_at_back` 或新建缓冲区）。
- 对比 `push_back`/`push_front` 里调用 `reserve_map_at_back`/`reserve_map_at_front` 时如何 `reallocate_map`，这是 deque 动态扩展的核心。

如果你在对照《STL源码剖析》看，接下来可以重点看 `push_back_aux`、`reallocate_map`、`reserve_map_at_back` 这几个函数，它们和这里的 `map_init` 是配套的。需要我接着讲 `reallocate_map` 吗？