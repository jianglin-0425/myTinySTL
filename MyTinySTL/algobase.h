#ifndef MYTINYSTL_ALGOBASE_H
#define MYTINYSTL_ALGOBASE_H

// 这个文件包含了 mystl 的基础算法

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl
{
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
    #pragma message("#undefing marco min")
#undef min
#endif // min

/*****************************************************************************************/
// max
// 取二者中的较大值，语义相等时保证返回第一个参数
/*****************************************************************************************/

template <class T>
const T& max(const T& lhs,const T& rhs)
{
    return lhs < rhs ? rhs:lhs;
}

// 重载版本使用函数对象 Comparator 代替比较操作
template <class T,class Comparator>
const T& max(const T& lhs,const T&rhs,Comparator comparator)
{
    return comparator(lhs,rhs) ? rhs:lhs;
}

/*****************************************************************************************/
// min
// 取二者中的较小值，语义相等时保证返回第一个参数
/*****************************************************************************************/
template <class T>
const T& min(const T& lhs,const T& rhs)
{
    return rhs < lhs ? rhs : lhs;
}
// 重载版本使用函数对象 Comparator 代替比较操作
template <class T,class Comparator>
const T& min(const T& lhs,const T&rhs,Comparator comparator)
{
    return comparator(lhs,rhs) ? rhs:lhs;
}
/*****************************************************************************************/
// iter_swap
// 将两个迭代器所指对象对调
/*****************************************************************************************/
template <class FIter1,class FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs)
{
    mystl::swap(*lhs,*rhs);
}

/*****************************************************************************************/
// copy
// 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter,class OutputIter>
OutputIter unchecked_copy_cat(InputIter first,InputIter last,OutputIter result,
                              mystl::input_iterator_tag)
{
    for (;  first!=last ; first++) {
        *result = * first;
    }
}
// ramdom_access_iterator_tag 版本
template <class RandomIter,class OutputIter>
OutputIter unchecked_copy_cat(RandomIter first,RandomIter last,OutputIter result,
                              mystl::random_access_iterator_tag)
{
    for (int n = last-first; n > 0; --n,++first,++result) {
        *result = *first;
    }
    return result;
}

template <class InputIter,class OutputIter>
OutputIter
unchecked_copy(InputIter first, InputIter last, OutputIter result)
{
    return unchecked_copy_cat(first,last,result,iterator_category(first));
}

}


#endif //MYTINYSTL_ALGOBASE_H
