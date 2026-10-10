#ifndef MYTINYSTL_CONSTRUCT_H
#define MYTINYSTL_CONSTRUCT_H

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

#include <new>

#include "type_traits.h"

namespace mystl
{
// construct 构造对象
template <class T>
void construct(T* ptr)
{
    ::new ((void*)ptr) T(); // 在已分配的原始内存上构造对象。它用的是 placement new
}

}


#endif //MYTINYSTL_CONSTRUCT_H
