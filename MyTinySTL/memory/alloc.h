//
// Created by user1 on 2026/3/22.
//
// 这个头文件包含一个类 alloc，用于分配和回收内存，以内存池的方式实现

#ifndef MYTINYSTL_ALLOC_H
#define MYTINYSTL_ALLOC_H

#include <new>

#include <cstddef>
#include <cstdio>

namespace mystl
{
// 共用体: FreeList
// 采用链表的方式管理内存碎片，分配与回收小内存（<=4K）区块
union FreeList
{
    union FreeList* next;   // 指向下一块区块
    char data[1];           // 存储本块内存的首地址
};

// 不同存储内存范围的上调大小
enum
{
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256
};
// 小对象的内存大小
enum{ ESmallObjectBytes = 4096};

// free lists 个数
enum {EFreeListsNumber = 56};

// 空间分配类 alloc
// 如果内存较大，超过 4096 bytes,直接调用 std::malloc,std::free
// 如果内存较小，以内存池管理，每次配置一大块内存，并维护对应的自由链表
class alloc
{
private:
    static char* start_free;    // 内存池起始位置
    static char* end_free;      // 内存池结束位置
    static size_t heap_size;    // 申请 heap 空间的附加值大小

    static FreeList* free_list[EFreeListsNumber]; //自由链表

public:
    static void* M_align(size_t bytes);
    static void* M_round
};

}


#endif //MYTINYSTL_ALLOC_H
