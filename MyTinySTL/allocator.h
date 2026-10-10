#ifndef MYTINYSTL_ALLOCATOR_H
#define MYTINYSTL_ALLOCATOR_H

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "util.h"

namespace mystl
{
// 模板类: allocator
// 模板函数代表数据类型
template <class T>
class Allocator
{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

public:
    static T* allocate();
    static T* allocate(size_type n);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr,size_type n);

    static void construct(T* ptr);
    static void construct(T* ptr,const T& value);
    static void construct(T* ptr,const T&& value);



};

template<class T>
T* Allocator<T>::allocate()
{
    return static_cast<T*>(::operator new(sizeof(T)));
}

template<class T>
T* Allocator<T>::allocate(size_type n)
{
    if(n==0)
        return nullptr;
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void Allocator<T>::construct(T *ptr)
{
    mystl::const
}

}


#endif //MYTINYSTL_ALLOCATOR_H
