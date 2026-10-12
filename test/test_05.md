# 一、new operator
这是语言内置的语法，是你写在代码里的那个 new：

```cpp
Foo* p = new Foo(1, 2);
```
它叫 new expression，是一个运算符，编译器看到它就展开成一套固定动作。它不能被重载（你没法改变 new 这个关键字的语法行为）。

new operator = 分配内存 + 调用构造函数，这两件事一起完成，缺一不可。

关键点：new operator 是"语法层"的东西，它内部去调用 operator new 来分配内存。

# 二、operator new(内存分配函数)
这是一个函数，可以被重载。它的职责只有一件事：分配原始内存，不调用构造函数。

```cpp
void* operator new(std::size_t size) {
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
```
它只分配内存，返回 void*，构造是 new operator 后面干的事。

# 三、placement new (定位new)
placement new 是 C++ 中一种在指定内存地址上构造对象的技术。它允许你把对象的构造和内存分配分开，是 STL 分配器、内存池、嵌入式等场景的核心工具
这是 operator new 的一个重载版本，特点是：它不分配内存，而是在你给的内存上构造对象。
```c++
// 在 <new> 里，注意这是 inline 的，不分配内存
void* operator new(std::size_t, void* ptr) noexcept {
    return ptr;   // 原样返回你给的指针
}
```
```c++
char buffer[sizeof(Foo)];              // 先有内存（栈、堆、内存池都行）
Foo* p = new (buffer) Foo(1, 2);       // 在 buffer 上构造 Foo
//        ↑
//   placement new：分配函数直接返回 buffer
//   然后在这块内存上调用 Foo 构造函数
```

# 四、注意点
### 1.内存对齐
```c++
alignas(Foo) char buf[sizeof(Foo)];   // ✅
char buf[sizeof(Foo)];                 // ⚠️ 可能未对齐，UB
```

### 2.必须手动调用析构函数
```c++
p->~Foo();   // 显式析构
```
### 3. 不要对 placement new 的对象用 delete
```c++
Foo* p = new (buf) Foo(42);
delete p;   // ❌ 灾难：会试图释放 buf，但 buf 不是 new 出来的
```

```text
Foo* p = new (buffer) Foo(1, 2);
└─────┬─────┘
│
┌────────┴─────────┐
│   new operator   │  ← 语法层，不可重载
│ （new 表达式）    │
└────────┬─────────┘
│ 拆成两步
┌────────┴────────────────────┐
│                             │
① 调 operator new              ② 调构造函数
（可重载的内存分配函数）          Foo::Foo(1,2)
│
┌────┴─────────────────────┐
│                          │
普通版：                    placement 版：
operator new(size)         operator new(size, void* ptr)
malloc + 抛异常             直接返回 ptr，不分配
*/
```