#include <iostream>
#include <new>

// ================ Demo 0 最小示例
class Foo{
public:
    int x;
    Foo(int v) : x(v) { std::cout << "构造 x=" << x << "\n"; }
    ~Foo() { std::cout << "析构 x=" << x << "\n"; }
};

void test1(){
    // 1. 预备原始内存（栈上，或 malloc，或内存池）
    char buff[sizeof(Foo)];

    // 2. 在这块内存上构造对象
    Foo* p = new (buff) Foo(42);

    std::cout << p->x << "\n";

    // 3. 必须手动调用析构（placement new 不会自动析构）
    p->~Foo();

    Foo* q = reinterpret_cast<Foo*>(buff);
    std::cout<<q->x<<"\n";

    // 4. 如果内存是 malloc 来的，还要 free
    // free(buf);  // 这里是栈数组，不用 free
}
// ================ Demo 1：复用同一块内存


int main()
{
    char buf[sizeof(Foo)];

    Foo* p =new (buf)Foo(1);    // 第一次
    p->~Foo();

    p = new(buf)Foo(2);         // 同一块内存， 第二次
    p->~Foo();

    return 0;
}