# 一、get_buffer_helper
一个带"逐次减半"重试的内存分配辅助函数。它要解决的是"大块内存分配失败后怎么办"
### 1. 上限检查
```c++
if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
    len = INT_MAX / sizeof(T);
```
为什么要限制？

malloc 的参数是 size_t（无符号）。但 INT_MAX 是 malloc 能保证分配的最大字节数（历史原因，某些平台 malloc 对超过 INT_MAX 的请求有未定义行为）。

计算：len * sizeof(T) 不能超过 INT_MAX，所以 len 不能超过 INT_MAX / sizeof(T)。

 **举例：**

* T = int（4 字节）：INT_MAX / 4 ≈ 536870911。如果请求 10 亿个 int，会被截断到 5.3 亿。
* T = char（1 字节）：INT_MAX / 1 = INT_MAX。

这是防御性编程——避免 malloc 在超大请求上崩溃或返回垃圾。


### 2.减半重试
因为大块内存分配可能失败，但小块可能成功。

**场景：内存碎片化**
系统有 1GB 空闲内存，但不连续：
```text
[已用 100MB][空闲 400MB][已用 300MB][空闲 200MB]...
```
请求 800MB 连续内存 → 失败（没有 800MB 连续块）。

但请求 400MB → 可能成功。

减半重试：800 → 400 → 200 → ... 总能找到一个能分配的大小
### 3.应用——容器扩容
vector 扩容时，如果申请不到理想大小，可以先用较小容量：
```cpp
ptrdiff_t want = new_capacity;
auto [buf, actual] = get_buffer_helper(want, (T*)nullptr);
if (buf == nullptr) {
throw std::bad_alloc();   // 完全失败才抛
}
// 用 actual 作为新容量（可能小于 want）
// 容器仍能工作，只是扩容幅度小一点
```
能接受：容器只要能工作就行，容量小一点不影响正确性，只是下次可能再扩容。
