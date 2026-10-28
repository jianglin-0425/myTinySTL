#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_
// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等

#include <cstddef>
#include "type_traits.h"

namespace mystl
{
// Move
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
    return static_cast<typename std::remove_reference<T>::type &&>(arg);
}

// forward
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
    return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type && arg) noexcept
{
    static_assert(!std::is_lvalue_reference<T>::value,"bad forward");
    return static_cast<T&&>(arg);
}

// swap

template <class Tp>
void swap(Tp& lhs,Tp& rhs)
{
    auto tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);
}

template <class ForwardIter1,class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1,ForwardIter1 last1,ForwardIter2 first2)
{
    for(;first1 != last1;++first1,(void)++first2){
        mystl::swap(*first1,*first2);
    }

    return first2;
}

template <class Tp,size_t N>
void swap(Tp(&a)[N],Tp(&b)[N])
{
    mystl::swap_range(a,a+N,b);
}

/********************************************************/
// pair
// 保存一对值 first 和 second

template <class T1,class T2>
struct pair
{
    typedef T1 first_type;
    typedef T2 second_type;

    T1 first;
    T2 second;

    pair():first(),second() {}

    pair(const T1& a,const T2& b):first(a),second(b) {}

    pair(const pair&) = default;
    pair(pair&&) = default;
    pair& operator=(const pair&) = default;
    pair& operator=(pair&&) = default;

    void swap(pair& other)
    {
        mystl::swap(first,other.first);
        mystl::swap(second,other.second);
    }
};

template <class T1,class T2>
bool operator==(const pair<T1,T2>& lhs,const pair<T1,T2>& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1,class T2>
bool operator!=(const pair<T1,T2>& lhs,const pair<T1,T2>& rhs)
{
    return !(lhs == rhs);
}

template <class T1,class T2>
bool operator<(const pair<T1,T2>& lhs,const pair<T1,T2>& rhs)
{
    return lhs.first < rhs.first ||
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

// make_pair：用两个值构造一个 pair
template <class T1,class T2>
pair<T1,T2> make_pair(T1&& first,T2&& second)
{
    return pair<T1,T2>(mystl::forward<T1>(first),mystl::forward<T2>(second));
}

}

#endif
