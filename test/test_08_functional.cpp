#include <iostream>

/**
 ## 1. `operator()` 是什么？

    `operator()` 叫**函数调用运算符重载**。
    重载之后，这个结构体的**实例可以像函数一样被调用**，所以这种结构体 / 类叫**函数对象（functor）**

     **末尾的 `const`**：代表这个成员函数是**常成员函数**
    >
    > 含义：调用这个 `operator()` 的时候，**不会修改函数对象本身**。
    > `plus` 这个结构体没有成员变量，对象本身状态是空的，所以必须加 `const`，保证可以用 `const plus<int>` 对象调用。
 */



 template <class T>
struct plus{
    T operator()(const T& x,const T&y) const
    {
        return x+y;
    }
};
void test1()
{
    plus<int> add;  //创建了一个函数对象

    // 方式1： 像函数一样调用
    int result = add(3,5);  // 实际调用add.operator()(3,5)
    std::cout<<"3+5="<<result<<std::endl;

    // 方式2: 显式调用
    int result2 = add.operator()(10,20);
    std::cout<<"10+20="<<result2<<std::endl;

    // 方式3: 创建临时对象直接调用
    int result3= plus<int>()(7,8);
    std::cout<<"7+8="<<result3<<std::endl;
}


// ============================= Demo2 ： hash函数测试
inline size_t bitwise_hash(const unsigned char* first,size_t count)
{
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull; // 初始值（哈希种子）
    const size_t fnv_prime = 1099511628211ull;         // 乘数（素数）
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif

    size_t result = fnv_offset;
    for (size_t i = 0; i < count; ++i) {
        result ^= (size_t) first[i];
        result *= fnv_prime;
    }
    return result;
}
template <class T>
size_t hash_value(const T& value)
{
    return bitwise_hash(reinterpret_cast<const unsigned char *>(&value),sizeof(T));
}
int main()
{
    std::cout<<"int 42:     "<<hash_value(42)<<"\n";
    std::cout<<"double 3.14:     "<<hash_value(3.14)<<"\n";
    std::cout<<"float 42:     "<<hash_value(3.14f)<<"\n";

    // 相同输入 ——> 相同输出
    std::cout<<"42 两次:     "<<hash_value(42)<<"  "<<hash_value(42)<<"\n";
    // 不同输入 ——> 不同输出 (大概率)
    std::cout<<"43 两次:     "<<hash_value(43)<<"\n";
}
