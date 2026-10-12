#include <iostream>
#include <cstring>
#include <utility>


// ============== Demo 1：move 会丢失左值属性，forward 不会 ==============
void process(int& x)  { std::cout << "左值引用\n"; }
void process(int&& x) { std::cout << "右值引用\n"; }

template <typename T>
void wrapper_move(T&& arg){
    // 即使你传入的是右值（比如 10），但在 wrapper 内部，
    // 参数 arg 是一个“有名字的变量”，所以 arg 本身变成了左值！
    std::cout << "wrapper_move:\n";
    process(std::move(arg)); // 无条件转右值
}

template <typename T>
void wrapper_forward(T&& arg){
    // 即使你传入的是右值（比如 10），但在 wrapper 内部，
    // 参数 arg 是一个“有名字的变量”，所以 arg 本身变成了左值！
    std::cout << "wrapper_move:\n";
    process(std::forward<T>(arg)); // 保持原样
}

void test_1(){
    int x = 1;
    std::cout << "=== 传左值给 move 版本 ===\n";
    wrapper_move(x);        // 应该走左值，但 move 把它变成右值了

    std::cout << "=== 传左值给 forward 版本 ===\n";
    wrapper_forward(x);     // 正确保留左值

    std::cout << "=== 传右值给 forward 版本 ===\n";
    wrapper_forward(5);     // 正确保留右值
}

// =================== Demo 2：完美转发的实际价值 —— 一个工厂函数 ==========
class Widget{
public:
    Widget(const std::string& s){
        std::cout << "  [拷贝构造] 参数: " << s << "\n";
    }
    Widget(std::string&& s) {
        std::cout << "  [移动构造] 参数: " << s << "\n";
    }

    Widget(int& x){
        std::cout << "  [左值构造] 参数: \n";
    }
    Widget(int&& x) {
        std::cout << "  [右值构造] 参数: \n";
    }
};

// 完美转发的工厂函数
template <class T, class... Args>
T* make(Args&&... args) {
    return new T(std::forward<Args>(args)...);
}

void test2(){
    std::string name = "hello";

    std::cout << "=== 传左值 name ===\n";
    Widget* a = make<Widget>(name);       // 应走拷贝

    std::cout << "=== 传右值临时对象 ===\n";
    Widget* b = make<Widget>("world");    // 应走移动

    delete a; delete b;
}
// =================== Demo 3：完美转发的实际价值 —— 一个工厂函数 ==========
template <class T1,class... Args>
void bad_construct(T1* p,Args&&... args)
{
    ::new ((void*)p) T1(args...);
}

template <class T1,class... Args>
void good_construct(T1* p,Args&&... args)
{
    ::new ((void*)p) T1(std::forward<Args>(args)...); // 使用forward
}

void test3(){
    char buf[sizeof(Widget)];
    Widget* p = reinterpret_cast<Widget*>(buf);

    std::cout << "bad_construct(p, 42):\n";
    bad_construct(p, 42);       // 传右值 42

    std::cout << "good_construct(p, 42):\n";
    good_construct(p, 42);      // 传右值 42
}
// =================== Demo 4：折叠规则 ==========
/*
        T& &    →  T&
        T& &&   →  T&
        T&& &   →  T&
        T&& &&  →  T&&
*/
template<class T>
void show(T&& arg)
{
    std::cout<<" T 是左值引用? "<<std::is_lvalue_reference<T>::value<<"\n";
    std::cout<<" T 是右值引用? "<<std::is_rvalue_reference<T>::value<<"\n";
    std::cout<<" 参数是左值引用? "<<std::is_lvalue_reference<decltype(arg)>::value<<"\n";
    std::cout<<" 参数是右值引用? "<<std::is_rvalue_reference<decltype(arg)>::value<<"\n";
}

int main() {

    int x = 5;
    std::cout<<"==== 传左值 x =====\n";
    show(x);
    // T = int&  → T&& 折叠成 int&

    std::cout << "=== 传右值 42 ===\n";
    show(42);
    // T = int   → T&& 就是 int&&

    std::cout << "=== 传 std::move(x) ===\n";
    show(std::move(x));
    // T = int   → T&& = int&&

    return 0;
}