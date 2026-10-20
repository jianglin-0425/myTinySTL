#include <iostream>
#include <cstddef>

struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// ================ Demo1 简化版: 测试 has_iterator_cat
template <class T>
struct has_iterator_cat
{
private:
    struct two{char a;char b;};
    template<class U> static two test(...);

    template<class U> static char test(typename U::iterator_category* = 0);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};


// 测试类型
struct RealIter {
    typedef int iterator_category;  // 有
    typedef int value_type;
    typedef int* pointer;
    typedef int& reference;
    typedef long difference_type;
};

struct NotIter {
    int x;                           // 没有
};
void test1(){
    std::cout << std::boolalpha;
    std::cout << "RealIter 有 iterator_category? "
              << has_iterator_cat<RealIter>::value << "\n";   // true
    std::cout << "NotIter 有 iterator_category? "
              << has_iterator_cat<NotIter>::value << "\n";    // false
    std::cout << "int 有 iterator_category? "
              << has_iterator_cat<int>::value << "\n";        // false
}

// ================ Demo2 简化版: 测试 iterator_traits_impl
template <class Iterator,bool>
struct iterator_traits_impl{};  // 空

template <class Iterator>
struct iterator_traits_impl<Iterator, true> {   // 五个别名
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::difference_type   difference_type;
};

template <class Iterator, bool = false>
struct my_traits : iterator_traits_impl<Iterator, false> {};

void test2(){
    // RealIter 走 true 版本
    using T = iterator_traits_impl<RealIter, true>;
    std::cout << "RealIter 萃取成功\n";
    // T::value_type 是 int

    // int 走 false 版本
    using T2 = iterator_traits_impl<int, false>;
    std::cout << "int 走空版本\n";
    // T2::value_type 不存在
}

// ================ Demo3 偏特化 =============


// 简化版 iterator_traits
template <class Iterator>
struct iterator_traits { /* 主模板，略 */ };

template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef T*                         pointer;
    typedef T&                         reference;
    typedef ptrdiff_t                  difference_type;
};

template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T                          value_type;
    typedef const T*                   pointer;
    typedef const T&                   reference;
    typedef ptrdiff_t                  difference_type;
};

void test3(){
    std::cout << std::boolalpha;

    // int*
    using T1 = iterator_traits<int*>;
    std::cout << "int* value_type 是 int? "
              << std::is_same<T1::value_type, int>::value << "\n";   // true
    std::cout << "int* reference 是 int&? "
              << std::is_same<T1::reference, int&>::value << "\n";   // true

    // const int*
    using T2 = iterator_traits<const int*>;
    std::cout << "const int* value_type 是 int? "
              << std::is_same<T2::value_type, int>::value << "\n";   // true（去掉 const）
    std::cout << "const int* value_type 是 const int? "
              << std::is_same<T2::value_type, const int>::value << "\n"; // false
    std::cout << "const int* reference 是 const int&? "
              << std::is_same<T2::reference, const int&>::value << "\n"; // true
}

// ==================== demo4: has_iterator_cat_of 判断某个类型的迭代器类别能否转换成指定 tag

// 简化版 iterator_traits
template <class T>
struct my_traits01 {
    typedef typename T::iterator_category iterator_category;
};

// has_iterator_cat（略，假设已有）

// has_iterator_cat_of
template <class T, class U, bool = true>
struct has_iterator_cat_of
        : std::is_convertible<typename my_traits01<T>::iterator_category, U> {};

// 测试类型
struct VecIter { typedef random_access_iterator_tag iterator_category; };
struct ListIter { typedef bidirectional_iterator_tag iterator_category; };
struct FwdIter  { typedef forward_iterator_tag iterator_category; };

void test4(){
    std::cout << std::boolalpha;

    std::cout << "VecIter 是 forward? "
              << has_iterator_cat_of<VecIter, forward_iterator_tag>::value << "\n";
    // true：random_access 能转 forward

    std::cout << "VecIter 是 random_access? "
              << has_iterator_cat_of<VecIter, random_access_iterator_tag>::value << "\n";
    // true

    std::cout << "ListIter 是 random_access? "
              << has_iterator_cat_of<ListIter, random_access_iterator_tag>::value << "\n";
    // false：bidirectional 不能转 random_access

    std::cout << "ListIter 是 forward? "
              << has_iterator_cat_of<ListIter, forward_iterator_tag>::value << "\n";
    // true
}

// ================= Demo5：反向跌打器的内部逻辑 ====================
#include <vector>
#include <iterator>
int main() {
    std::vector<int> v= {1,2,3,4,5};

    // 手动reverset
    std::reverse_iterator<std::vector<int>::iterator> rit(v.end());

    std::cout<<"反向遍历: ";
    while (rit != std::reverse_iterator<std::vector<int>::iterator>(v.begin()))
    {
        std::cout<<*rit<<" ";
        ++rit;
    }

    std::cout << "\n";
    // 输出: 5 4 3 2 1

    // 验证 current 的位置
    std::vector<int>::iterator forward = v.end();
    std::reverse_iterator<std::vector<int>::iterator> r(forward);
    std::cout<< "r.base()?"<<(r.base() == v.end()) << "\n";  // true
    std::cout<<"*r= "<<*r << "\n";      // 5 (*(end()-1))
}

