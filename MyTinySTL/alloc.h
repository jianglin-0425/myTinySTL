#ifndef MYTINYSTL_ALLOC_H
#define MYTINYSTL_ALLOC_H

// 这个头文件包含一个类 alloc，用于分配和回收内存，以内存池的方式实现
//
// 从 v2.0.0 版本开始，将不再使用内存池，这个文件将被弃用，但暂时保留
//
// 注意！！！
// 我知道这个文件里很多实现是错的，这是很久很久前写的了，后面已经不用这个东西了，
// 所以我也没再维护，有诸多问题，已经有人在issue中都提了，free_list的修改，
// 指针作为参数时没实际修改到原指针，等等。相信会看这么仔细的，大部分都是
// 初学C++的朋友，大佬都不会看这些玩具了，所以其中的错误，就留给对内存池实现
// 感兴趣的朋友去修改啦！

#include <new>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
namespace mystl{

// 不同内存范围的上调大小
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
enum { ESmallObjectBytes = 4096};

// free lists 个数
enum { EFreeListsNumber = 56 };

// 共用体: FreeList
// 采用链表的方式管理内存碎片，分配与回收小内存(<=4k)区域
union FreeList
{
    union FreeList* next; // 指向下一区域
    char data[1];  // 存储本块区域的首地址
};

// 空间配置器 alloc
// 如果内存较大，超过4096 bytes，直接调用std::malloc, std::free
// 当内存较小时，以内存池管理，每次配置一大块内存，并维护对应的自由链表
class Alloc
{
private:
    static char * start_free;   // 内存池起始位置
    static char* end_free;  //内存池结束位置
    static size_t heap_size; //申请 heap 空间附加值大小

    static FreeList* free_list[EFreeListsNumber]; // 自由链表

public:
    static void* allocate(size_t n);
    static void deallocate(void* p,size_t n );
    static void* reallocate(void* p,size_t old_size,size_t new_size);

private:
    static size_t M_align(size_t bytes);
    static size_t M_round_up(size_t bytes);
    static size_t M_freelist_index(size_t bytes);
    static void* M_refill(size_t n);
    static char* M_chunk_alloc(size_t size,size_t &nblock);
};

// 静态初始化
char* Alloc::start_free = nullptr;
char* Alloc::end_free = nullptr;
FreeList* Alloc::free_list[EFreeListsNumber] = {
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
};

// 分配大小为 n 的空间， n > 0
void* Alloc::allocate(size_t n)
{
    FreeList* my_free_list;
    FreeList* result;
    if(n > static_cast<size_t>(ESmallObjectBytes)) // 大对象 malloc 直接分配
        return std::malloc(n);
    my_free_list = free_list[M_freelist_index(n)];
    result = my_free_list;
    if(result== nullptr){
        void* r = M_refill(M_round_up(n));
        return r;
    }
    my_free_list = result->next;
    return result;

}
// 释放 p 指向的大小为 n 的空间, p 不能为 0
void Alloc::deallocate(void *p, size_t n)
{
    if(n>static_cast<size_t>(ESmallObjectBytes))
    {
        std::free(p);
        return;
    }
    FreeList* q = reinterpret_cast<FreeList*>(p);
    size_t index = M_freelist_index(n);
    q->next = free_list[index]; // 新节点指向原头
    free_list[index] = q;       // 写回数组，新节点成为头
}

/**
 * 重新分配空间
 * @param p  指向新空间的指针
 * @param old_size  原来空间的大小
 * @param new_size  申请空间的大小
 * @return
 */
void* Alloc::reallocate(void *p, size_t old_size, size_t new_size)
{
    deallocate(p,old_size);
    p = allocate(new_size);
    return p;

}
// bytes 对应上调大小
size_t Alloc::M_align(size_t bytes)
{
    if (bytes <= 128)
    {
        return EAlign128;      // 8
    }
    else if (bytes <= 256)
    {
        return EAlign256;      // 16
    }
    else if (bytes <= 512)
    {
        return EAlign512;      // 32
    }
    else if (bytes <= 1024)
    {
        return EAlign1024;     // 64
    }
    else if (bytes <= 2048)
    {
        return EAlign2048;     // 128
    }
    else
    {
        return EAlign4096;     // 256
    }
}

// 把 bytes 向上取整到 M_align(bytes) 的倍数
size_t Alloc::M_round_up(size_t bytes)
{
    return ((bytes + M_align(bytes) - 1) & ~(M_align(bytes) - 1));
}

// 重新填充: free list
/**
 *
 * M_chunk_alloc 从内存池批发大块，
 * M_refill 把批发来的大块切成小块、串成链表，挂到 free-list 上。
 *
 * 切成 10 个小块，串成链表:  第 1 个返回给调用者,剩下 9 个挂到 free-list
 * @param n
 * @return
 */
void * Alloc::M_refill(size_t n)
{
    size_t  nblock = 10;
    char* c = M_chunk_alloc(n,nblock); // nblock是引用传递
    FreeList* my_free_list;
    FreeList* result,*cur,*next;
    // 如果只有一个区块，就把这个区块返回给调用者，free list 没有增加新节点
    if(nblock==1)
        return c;
    // 否则把一个区块给调用者，剩下的纳入 free list 作为新节点
    my_free_list = free_list[M_freelist_index(n)];
    result = (FreeList*) c;
    my_free_list = next =(FreeList*)(c+n);
    for(size_t i=1;;++i)
    {
        cur =next;
        next = (FreeList*)((char *)next+n);
        if(nblock - 1 ==i)
        {
            cur->next = nullptr;
            break;
        }
        else
        {
            cur->next = next;
        }
    }
    return result;
}



/**
 * // 根据申请的字节数 bytes，返回对应的 free-list 索引
 * // free-list 共 64 档，每档负责固定大小的内存块
 * @param bytes
 * @return int
 */
size_t Alloc::M_freelist_index(size_t bytes)
{
    // 段1：1 ~ 128 字节，按 8 字节对齐，占索引 0~15（共16档）
    if (bytes <= 128)
    {
        return (bytes + EAlign128 - 1) / EAlign128 - 1;
    }
        // 段2：129 ~ 256 字节，按 16 字节对齐，占索引 16~23（共8档）
    else if (bytes <= 256)
    {
        return 15 + (bytes + EAlign256 - 129) / EAlign256;
    }
        // 段3：257 ~ 512 字节，按 32 字节对齐，占索引 24~31（共8档）
    else if (bytes <= 512)
    {
        return 23 + (bytes + EAlign512 - 257) / EAlign512;
    }
        // 段4：513 ~ 1024 字节，按 64 字节对齐，占索引 32~39（共8档）
    else if (bytes <= 1024)
    {
        return 31 + (bytes + EAlign1024 - 513) / EAlign1024;
    }
        // 段5：1025 ~ 2048 字节，按 128 字节对齐，占索引 40~47（共8档）
    else if (bytes <= 2048)
    {
        return 39 + (bytes + EAlign2048 - 1025) / EAlign2048;
    }
        // 段6：2049 ~ 4096 字节，按 256 字节对齐，占索引 48~55（共8档）
    else
    {
        return 47 + (bytes + EAlign4096 - 2049) / EAlign4096;
    }
}

// 从内存池中取空间给 free list 使用，条件不允许时，会调整 nblock
char* Alloc::M_chunk_alloc(size_t size,size_t& nblock)
{
    char* result;
    size_t need_bytes = size * nblock;
    size_t pool_bytes = end_free - start_free;

    if(pool_bytes >= need_bytes){
        // 如果内存池剩余大小完全满足需求量，返回他
        result = start_free;
        start_free +=need_bytes;
        return result;
    }
    else if(pool_bytes>=size){
        // 如果内存池剩余大小不能完全满足需求量，但至少可以分配一个或一个以上的区块，就返回它
        nblock = pool_bytes /size;
        need_bytes = size * nblock;
        result = start_free;
        start_free +=need_bytes;
        return result;
    }
    else{
        // 如果内存池剩余大小连一个区块都无法满足
        if(pool_bytes>0){
            // 如果内存池还有剩余，把剩余的空间加入到 free list 中
            FreeList* my_free_list = free_list[M_freelist_index(pool_bytes)];
            ((FreeList*)start_free)->next = my_free_list;
            free_list[M_freelist_index(pool_bytes)]= (FreeList *) start_free;
        }
        // 申请 heap 空间
        // 为什么申请2 倍还加 heap_size/16？？？
        //      2 倍：一次多要点，减少后续 malloc 次数（类似 vector 扩容策略）。
        //      heap_size/16：堆越大，追加越多，叫做"启发式增长"。这样内存池能应对越来越大的需求
        size_t bytes_to_get = (need_bytes<<1) + M_round_up(heap_size >> 4);
        start_free = (char*) std::malloc(bytes_to_get); // 向系统申请一大块新内存
        if(!start_free) // malloc 失败说明系统内存紧张。但可能别的 free-list 里还有更大的块
        {   // heap 空间不够
            FreeList* my_free_list, *p;
            // 试着查找有无未用区块，且区块足够大的free_list
            for(size_t i=size;i<=ESmallObjectBytes;i+= M_align(i))
            {
                my_free_list = free_list[M_freelist_index(i)];
                p = my_free_list;
                if(p)
                {
                    my_free_list = p->next;
                    start_free = (char *)p;
                    end_free = start_free + i;
                    return M_chunk_alloc(size,nblock);
                }
            }
            std::printf("out of memory");
            end_free = nullptr;
            throw std::bad_alloc();
        }
        end_free = start_free +bytes_to_get;
        heap_size += bytes_to_get;
        return M_chunk_alloc(size,nblock);
    }
}

}


#endif //MYTINYSTL_ALLOC_H
