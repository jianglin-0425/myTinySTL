#ifndef MYTINYSTL_ALLOCATOR_H
#define MYTINYSTL_ALLOCATOR_H

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
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
    // 申请内存
    static T* allocate();
    static T* allocate(size_type n);

    // 撤销内存
    static void deallocate(T* ptr);
    static void deallocate(T* ptr,size_type n);


    // 调用构造
    static void construct(T* ptr);
    static void construct(T* ptr,const T& value);
    static void construct(T* ptr,T&& value);

    template<class... Args>
    static void construct(T* ptr,Args&& ...args);

    static void destroy(T* ptr);
    static void destroy(T* first,T* last);
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
void Allocator<T>::deallocate(T *ptr)
{
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}
template <class T>
void Allocator<T>::deallocate(T *ptr,size_type)
{
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void Allocator<T>::construct(T *ptr)
{
    mystl::construct(ptr);
}

template <class T>
void Allocator<T>::construct(T *ptr,const T& value)
{
    mystl::construct(ptr,value);
}

template <class T>
void Allocator<T>::construct(T *ptr, T&& value)
{
    mystl::construct(ptr,mystl::move(value));
}

template <class T>      // 类的模板
template <class ...Args>    // 函数的模板
void Allocator<T>::construct(T* ptr,Args&&...args)
{
    mystl::construct(ptr,mystl::forward<Args>(args)...);
}

template <class T>
void Allocator<T>::destroy(T *ptr)
{
    mystl::destory(ptr);
}

template <class T>
void Allocator<T>::destroy(T *first,T* last)
{
    mystl::destory(first,last);
}


}


#endif //MYTINYSTL_ALLOCATOR_H
