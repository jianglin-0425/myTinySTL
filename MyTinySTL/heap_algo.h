#ifndef MYTINYSTL_HEAP_ALGO_H
#define MYTINYSTL_HEAP_ALGO_H

// 这个头文件包含 heap 的四个算法： push_heap, pop_heap,sort_heap,make_heap
#include "iterator.h"

namespace mystl
{
/*****************************************************************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*****************************************************************************************/
/**
 一、功能：把新元素"上浮"到正确位置
    在大顶堆（max-heap）里插入一个新元素：
        1.新元素先放在末尾
        2.与父节点比较，如果比父节点大，就和父节点交换
        3.重复直到满足堆性质（父 ≥ 子）或到达堆顶

    这个函数做的就是"上浮"过程。
 */
template <class RandomIter,class Distance,class T>
void push_heap_aux(RandomIter first,Distance holdIndex,Distance topIndex, T value)
{
    auto parent = (holdIndex - 1)/2; // 父亲节点
    while (holdIndex > topIndex && *(first + parent) < value)
    {
        // 使用 operator < ,所以 heap 为 max - heap
        *(first + holdIndex) = *(first+parent);
        holdIndex = parent;
        parent = (holdIndex - 1)/2;
    }
    *(first + holdIndex) = value;
}
template <class RandomIter,class Distance>
void push_heap_d(RandomIter first,RandomIter last,Distance*)
{
    mystl::push_heap_aux(first,(last-first)-1,static_cast<Distance>(0),*(last-1));
}
template <class RandomIter>
void push_heap(RandomIter first,RandomIter last)
{
    // 新元素应该已置于底部容器的最尾端
    mystl::push_heap_d(first,last, distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter ,class Distance,class T,class Comparator>
void push_heap_aux(RandomIter first,Distance holeIndex,
                   Distance topIndex,T value,Comparator comp)
{
    auto parent = (holeIndex-1) /2;
    while (holeIndex > topIndex && comp(*(first + parent),value))
    {
        *(first+holeIndex) = *(first+parent);
        holeIndex = parent;
        parent = (holeIndex - 1) /2;
    }
    *(first + holeIndex) = value;
}

template <class RandomIter,class Comparator,class Distance>
void push_heap_d(RandomIter first,RandomIter last,Distance*,Comparator comp)
{
    mystl::push_heap_aux(first,(last-first)-1,
                         static_cast<Distance>(0),*(last-1),comp);
}

template <class RandomIter,class Comparator>
void push_heap(RandomIter first,RandomIter last,Comparator comp)
{
    mystl::push_heap_d(first,last, distance_type(first),comp);
}

/*****************************************************************************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
/*****************************************************************************************/
template <class RandomIter,class T,class Distance>
void adjust_heap(RandomIter first,Distance holeIndex,Distance len,T value)
{
    // 先进行下溯( percolate down) 过程
    auto topIndex = holeIndex;
    auto rchild = 2*holeIndex +2;
    while (rchild < len)
    {
        if(*(first + rchild) < *(first + rchild -1))
            --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2*(rchild+1);
    }
    if(rchild == len)
    {
        // 如果没有右子节点
        *(first + holeIndex) = *(first + (rchild-1));
        holeIndex = rchild -1;
    }
    // 再执行一次上溯(percolate up)过程
    mystl::push_heap_aux(first,holeIndex,topIndex,value);
}

template <class RandomIter,class T,class Distance>
void pop_heap_aux(RandomIter first,RandomIter last,
                  RandomIter result,T value,Distance*)
{
    // 先将首值调至尾节点，然后调整[first,last-1) 使之成为一个 max-heap
    *result = *first;
    mystl::adjust_heap(first,static_cast<Distance>(0),last-first,value);
}

template <class RandomIter>
void pop_heap(RandomIter first,RandomIter last)
{
    mystl::pop_heap_aux(first,last-1,last-1,*(last-1), distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter,class T,class Distance,class Comparator>
void adjust_heap(RandomIter first,Distance holeIndex,
                 Distance len,T value,Comparator comp)
{
    // 先进行下溯( percolate down) 过程
    auto topIndex = holeIndex;
    auto rchild = 2*holeIndex +2;
    while (rchild < len)
    {
        if(comp(*(first + rchild) , *(first + rchild -1)))
            --rchild;
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = 2*(rchild+1);
    }
    if(rchild == len)
    {
        // 如果没有右子节点
        *(first + holeIndex) = *(first + (rchild-1));
        holeIndex = rchild -1;
    }
    // 再执行一次上溯(percolate up)过程
    mystl::push_heap_aux(first,holeIndex,topIndex,value,comp);
}

template <class RandomIter,class T,class Distance,class Comparator>
void adjust_heap_aux(RandomIter first,RandomIter last,RandomIter result,
                     T value,Distance* ,Comparator comp)
{
    *result = *first;   // 先将尾指针设置为首值，即尾指针为欲求结果
    mystl::adjust_heap(first,static_cast<Distance>(0),last-first,value,comp);
}

template <class RandomIter,class Comparator>
void pop_heap(RandomIter first,RandomIter last,Comparator comp)
{
    mystl::pop_heap_aux(first,last-1,last-1,*(last-1),
                        distance_type(first),comp);
}

/*****************************************************************************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
/*****************************************************************************************/
template <class RandomIter>
void sort_heap(RandomIter first,RandomIter last)
{
    // 每执行一次 pop_heap, 最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序
    while (last - first>1)
    {
        mystl::pop_heap(first,last--);
    }
}
// 重载版本使用函数对象 comp 代替比较操作
template <class RanomIter,class Comparator>
void sort_heap(RanomIter first,RanomIter last,Comparator comp)
{
    while (last-first > 1)
    {
        mystl::pop_heap(first,last--,comp);
    }
}

/*****************************************************************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/*****************************************************************************************/
template <class RandomIter,class Distance>
void make_heap_aux(RandomIter first,RandomIter last,Distance*)
{
    if(last - first <2)
        return;
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    while (true)
    {
        //重排以 holeIndex 为首的子树
        mystl::adjust_heap(first,holeIndex,len,*(first+holeIndex));
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

template <class RandomIter>
void make_heap(RandomIter first,RandomIter last)
{
    mystl::make_heap_aux(first,last, distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作

template <class RandomIter,class Distance,class Comparator>
void make_heap_aux(RandomIter first,RandomIter last,Distance*,Comparator comp)
{
    if(last - first <2)
        return;
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    while (true)
    {
        //重排以 holeIndex 为首的子树
        mystl::adjust_heap(first,holeIndex,len,*(first+holeIndex),comp);
        if(holeIndex == 0)
            return;
        holeIndex--;
    }
}

template <class RandomIter,class Comparator>
void make_heap(RandomIter first,RandomIter last,Comparator comp)
{
    mystl::make_heap_aux(first,last, distance_type(first),comp);
}
}
#endif //MYTINYSTL_HEAP_ALGO_H
