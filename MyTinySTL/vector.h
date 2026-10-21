#ifndef MYTINYSTL_VECTOR_H
#define MYTINYSTL_VECTOR_H


// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
//
// 异常保证：
// mystl::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>
#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"

namespace mystl
{
#ifdef  max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

// 模板类: vector
// 模板参数 T 代替类型
template <class T>
class vector
{
    static_assert(!std::is_same<bool,T>::value,"vector<bool> is abandoned in mystl");
public:
    // vector 的嵌套型定义
    typedef mystl::Allocator<T>                             allocator_type;
    typedef mystl::Allocator<T>                             data_allocator;

    typedef typename allocator_type::value_type             value_type;
    typedef typename allocator_type::pointer                pointer;
    typedef typename allocator_type::const_pointer          const_pointer;
    typedef typename allocator_type::reference              reference;
    typedef typename allocator_type::const_reference        const_reference;
    typedef typename allocator_type::size_type              size_type;
    typedef typename allocator_type::difference_type        difference_type;

    typedef value_type*                                     iterator;
    typedef const value_type*                               const_iterator;
    typedef mystl::reverse_iterator<iterator>               reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator>         const_reverse_iterator;

    allocator_type get_allocator(){ return data_allocator(); }

private:
    iterator begin_;    // 表示目前使用控件的头部
    iterator end_;      // 表示目前使用控件的尾部
    iterator cap_;      // 表示目前存储控件的尾部

public:
    // 构造、复制、移动、析构函数
    vector() noexcept
    { try_init(); }

    explicit vector(size_type n)
    { fill_init(n,value_type());}

    template<class Iter,typename std::enable_if<
    mystl::is_input_iterator<Iter>::value,int>::type = 0>
    vector(Iter first,Iter last)
    {
        MYSTL_DEBUG(!(last < first));
        range_init(first,last);
    }
private:
    // helper functions

    // initialize / destroy
    void try_init() noexcept;
    void init_space(size_type size,size_type cap);
    void fill_init(size_type n,const value_type& value);

    template<class Iter>
    void range_init(Iter first,Iter last);

    void destory_recover(iterator first,iterator last,size_type n);


    // 计算增长的尺寸
    size_type get_new_cap(size_type add_size);

    // 分配
    void fill_assign(size_type n,const value_type& value);

    template<class Iter>
    void copy_assign(Iter first,Iter last,input_iterator_tag);

    template<class Iter>
    void copy_assign(Iter first,Iter last,random_access_iterator_tag);


    // 重新分配
    template<class... Args>
    void reallocate_emplace(iterator pos,Args&& ... args);

    void reallocate_insert(iterator pos,const value_type& value);

    // 插入
    iterator fill_insert(iterator pos,size_type n,const value_type& value);

    template<class Iter>
    void copy_insert(iterator pos,Iter first,Iter last);

    // 收缩至适配
    void reinsert(size_type size);
};

/*****************************************************************************************/
// helper function
// try_init 函数，若分配失败则忽略，不抛出异常
template<class T>
void vector<T>::try_init() noexcept
{
    try
    {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    }
    catch(...)
    {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}
// init_space 函数
template <class T>
void vector<T>::init_space(size_type size, size_type cap)
{
    try
    {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    }
    catch (...)
    {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
    }
}
// fill_init 函数
template <class T>
void vector<T>::fill_init(size_type n, const value_type &value)
{
    const size_type init_size = mystl::max(static_cast<size_type>(16),n);
    init_space(n,init_size);
    mystl::uninitialized_fill_n(begin_,n,value);
}

// range_init 函数
//template <class T>
}
#endif //MYTINYSTL_VECTOR_H
