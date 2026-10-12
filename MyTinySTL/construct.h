#ifndef MYTINYSTL_CONSTRUCT_H
#define MYTINYSTL_CONSTRUCT_H

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

#include <new>

#include "type_traits.h"
#include "util.h"

namespace mystl
{
// construct 构造对象
template <class T>
void construct(T* ptr)
{
    ::new ((void*)ptr) T(); // 在已分配的原始内存上构造对象。它用的是 placement new
}
template <class T1,class T2>
void construct(T1* ptr,T2& val)
{
    ::new((void*)ptr)T1(val);
}
template<class T1,class... Args>
void construct(T1* ptr,Args&&... args)
{
    ::new ((void*)ptr) T1(mystl::forward<Args>(args)...);
}

// destory 将对象析构
template <class T1>
void destory_one(T1*,std::true_type){}

template <class T1>
void destory_one(T1* pointer,std::false_type )
{
    if(pointer!= nullptr)
    {
        pointer->~T1();
    }
}

template <class ForwardIter>
void destory_cat(ForwardIter,ForwardIter,std::true_type ){}

template <class ForwardIter>
void destory_cat(ForwardIter first,ForwardIter last,std::false_type)
{
    for (; first !=last; ++first) {
        destory(&*first);
    }   
}

template <class T1>
void destory(T1* pointer)
{
    destory_one(pointer,std::is_trivially_destructible<T1>{});
}

template <class ForwardIter>
void destory(ForwardIter first,ForwardIter last)
{
    destory_cat(first,last,std::is_trivially_destructible<
            typename iterator_traits<ForwardIter>::value_type>{});
}

}


#endif //MYTINYSTL_CONSTRUCT_H
