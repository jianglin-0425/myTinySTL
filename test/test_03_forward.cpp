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
};

// 完美转发的工厂函数
template <class T, class... Args>
T* make(Args&&... args) {
    return new T(std::forward<Args>(args)...);
}

int main() {
    std::string name = "hello";

    std::cout << "=== 传左值 name ===\n";
    Widget* a = make<Widget>(name);       // 应走拷贝

    std::cout << "=== 传右值临时对象 ===\n";
    Widget* b = make<Widget>("world");    // 应走移动

    delete a; delete b;

    return 0;
}