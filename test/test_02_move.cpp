#include <iostream>
#include <cstring>
#include <utility>


/**
 * move 在运行时什么都不做。它编译后完全消失，不产生任何指令。它纯粹是编译期的类型转换。
        ```cpp
            void f(int&);    // 重载 A
            void f(int&&);   // 重载 B

            int a = 5;
            f(a);              // 选 A，因为 a 是左值
            f(std::move(a));   // 选 B，因为 move 返回右值引用
        ```
        编译器在类型检查阶段就知道该选 A 还是 B。选完之后，move 就完成了使命，被丢弃。运行时没有任何痕迹。

        所以你问"move 底层怎么做"——第一层答案是：它只活在编译期，运行时不存在
 *
 *
 *
 */
template <class T>
typename std::remove_reference<T>::type&& my_move(T&& arg) noexcept
{
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}
// ======================Demo1: 最小例子 —— 看 move 只是类型转换 ========================
void test1(){
    // 重点：x 依然是 10。move 没动它一根毫毛。它只是把 x 的"身份标签"临时改成了右值引用。
    int x = 10;

    // 1. 传入左值
    auto&& r1 = my_move(x);
    std::cout << "my_move(x) ret is right reference? "
              << std::is_rvalue_reference<decltype(r1)>::value << "\n";

    // 2. 传入右值
    auto&& r2 = my_move(42);
    std::cout << "my_move(42) ret is right reference? "
              << std::is_rvalue_reference<decltype(r2)>::value << "\n";

    // 3. 关键：x 本身没有任何变化
    std::cout << "After call my_move: x = " << x << "\n";

}

// ======================Demo2:move 的真正价值 —— 触发移动构造 ========================
class MyString{
public:
    char* data_;
    size_t size_;

    // 普通构造: 分配内存，初始化数据
    MyString(const char* s){
        size_ = std::strlen(s);
        data_ = new char[size_ + 1];
        std::strcpy(data_, s);
        std::cout << "  [构造] 分配内存: " << data_ << "\n";
    }

    // 拷贝构造: 重新分配内存 + 复制资源内容,原对象资源保留
    MyString(const MyString& other){
        size_ = other.size_;
        data_ = new char[size_ + 1];
        std::strcpy(data_, other.data_);
        std::cout << "  [拷贝构造] 重新分配内存\n";
    }
    // 移动构造：直接偷指针
    MyString(MyString&& other) noexcept{
        data_ = other.data_;       // 偷过来
        size_ = other.size_;
        other.data_ = nullptr;     // 把源对象置空
        other.size_ = 0;
        std::cout << "  [移动构造] 偷走指针，无分配\n";
    }
    ~MyString() { delete[] data_; }
};

// =============== Demo 2：move 的真正价值 —— 触发移动构造
void test2(){

    MyString a("hello world");
    std::cout << "--- 用拷贝构造创建 b ---\n";
    MyString b = a;                 // a 是左值 → 拷贝

    std::cout << "--- 用移动构造创建 c ---\n";

    MyString c = std::move(a);      // move(a) 转成右值 → 移动

    std::cout << "--- a 现在: " << (a.data_ ? a.data_ : "(空)") << "\n";
    std::cout << "--- b 现在: " << (b.data_ ? b.data_ : "(空)") << "\n";
    std::cout << "--- c 现在: " << (c.data_ ? c.data_ : "(空)") << "\n";
}

// ============= Demo 3：手动展开 move 的推导过程
// 模拟 move 内部，把每一步类型打印出来
template <class T>
void explain(T&& arg){
    std::cout << "  T 推导为:               "
              << (std::is_lvalue_reference<T>::value ? "左值引用" :
                  std::is_rvalue_reference<T>::value ? "右值引用" : "非引用")
              << "\n";

    std::cout << "  remove_reference<T>:    "
              << (std::is_same<typename std::remove_reference<T>::type, int>::value
                  ? "int" : "其它")
              << "\n";

    using ReturnType = typename std::remove_reference<T>::type&&;
    std::cout << "  返回类型 = int&&:       "
              << std::is_rvalue_reference<ReturnType>::value
              << "\n";
}

void test3(){
    // 左值输入 → T = int& → remove_reference 剥成 int → 返回 int&&
    //
    //右值输入 → T = int → remove_reference 不动 → 返回 int&&
    //
    //两条路殊途同归，都得到 int&&。这就是 remove_reference 存在的意义。

    int x = 5;

    std::cout << "=== 传入左值 x ===\n";
    explain(x);       // T = int&

    std::cout << "=== 传入右值 42 ===\n";
    explain(42);      // T = int
}
// ================= demo4 反列，去掉 remove_reference 会怎样


// 错误版本：不剥引用
template <class T>
T&& bad_move(T&& arg) noexcept {
    return static_cast<T&&>(arg);   // 直接用 T&&
}
int main() {

    int x = 5;

    // 传入左值：T = int&
    // T&& = int& && = int&  （引用折叠！）
    auto&& r = bad_move(x);
    std::cout << "bad_move(x) 是右值引用? "
              << std::is_rvalue_reference<decltype(r)>::value << "\n";
    // 输出 0 —— 失败！本来应该得到右值引用，结果还是左值引用

    return 0;
}