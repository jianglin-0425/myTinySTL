#ifndef MYTINYSTL_LIST_H
#define MYTINYSTL_LIST_H

// 这个头文件包含了一个模板类 list
// list : 双向链表

// notes:
//
// 异常保证：
// mystl::list<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "functional.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl{
template <class T> struct list_node_base;
template <class T> struct list_node;

template <class T>
struct node_traits
{
    typedef list_node_base<T> * base_ptr;
    typedef list_node<T>*       node_ptr;
};

// list 的节点结构
template <class T>
struct list_node_base
{
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;

    base_ptr prev;  // 前一节点
    base_ptr next;  // 下一节点

    list_node_base() = default;
    node_ptr as_node()
    {
        return static_cast<node_ptr>(self());
    }
    void unlink()
    {
        prev = next = self();
    }
    base_ptr self()
    {
        return static_cast<base_ptr>(&*this);
    }
};

template <class T>
struct list_node:public list_node_base<T>
{
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;

    T value ;   // 数据域

    list_node() = default;
    list_node(const T& v):value(v)
    {
    }
    base_ptr as_base()
    {
        return static_cast<base_ptr>(&*this);
    }
    node_ptr self()
    {
        return static_cast<node_ptr>(&*this);
    }
};

// list 的迭代器设计
template <class T>
struct list_iterator: public mystl::iterator<mystl::bidirectional_iterator_tag,T>
{
    typedef T                       value_type;
    typedef T*                      pointer;
    typedef T&                      reference;
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;
    typedef list_iterator<T>        self;

    base_ptr node_;     //指向当前节点

    // 构造函数
    list_iterator() = default;
    list_iterator(base_ptr x):node_(x){}

    list_iterator(node_ptr x):node_(x->as_base()){}

    list_iterator(const list_iterator& rhs):node_(rhs.node_)
    {}

    // 重载操作符
    reference  operator*()const { return  node_->as_node()->value; }
    pointer operator->() const { return &(operator*()) ; }

    self& operator++()
    {
        MYSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        MYSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }

    //重载比较运算符
    bool operator==(const self& rhs) const { return node_ == rhs.node_;}
    bool operator!=(const self& rhs) const { return  node_ != rhs.node_; }

};

template <class T>
struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>
{
    typedef T                                 value_type;
    typedef const T*                          pointer;
    typedef const T&                          reference;
    typedef typename node_traits<T>::base_ptr base_ptr;
    typedef typename node_traits<T>::node_ptr node_ptr;
    typedef list_const_iterator<T>            self;

    base_ptr node_;

    list_const_iterator() = default;
    list_const_iterator(base_ptr x)
            :node_(x) {}
    list_const_iterator(node_ptr x)
            :node_(x->as_base()) {}
    list_const_iterator(const list_iterator<T>& rhs)
            :node_(rhs.node_) {}
    list_const_iterator(const list_const_iterator& rhs)
            :node_(rhs.node_) {}

    reference operator*()  const { return node_->as_node()->value; }
    pointer   operator->() const { return &(operator*()); }

    self& operator++()
    {
        MYSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--()
    {
        MYSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // 重载比较操作符
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};

// 模板类: list
// 模板参数 T 代表 数据类型
template <class T>
class list
{
public:
    // list的嵌套型别定义
    typedef mystl::Allocator<T>         allocator_type;
    typedef mystl::Allocator<T>         data_allocator;
    typedef mystl::Allocator<list_node_base<T>> base_allocator;
    typedef mystl::Allocator<list_node<T>>  node_allocator;

    typedef typename allocator_type::value_type             value_type;
    typedef typename allocator_type::pointer                pointer;
    typedef typename allocator_type::const_pointer          const_pointer;
    typedef typename allocator_type::reference              reference;
    typedef typename allocator_type::const_reference        const_reference;
    typedef typename allocator_type::size_type              size_type;
    typedef typename allocator_type::difference_type        difference_type;

    typedef list_iterator<T>                                iterator;
    typedef list_const_iterator<T>                          const_iterator;
    typedef mystl::reverse_iterator<iterator>               reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator>         const_reverse_iterator;

    typedef typename node_traits<T>::base_ptr           base_ptr;
    typedef typename node_traits<T>::node_ptr           node_ptr;

    allocator_type get_allocator()  { return node_allocator();}

private:
    base_ptr node_;     // 指向末尾节点
    size_type size_;    // 大小

public:
    // 构造、复制、移动、析构函数
private:
    // 帮助函数

    // 创建/销毁节点
    template<class ...Args>
    node_ptr create_node(Args&& ...args);
    void destory_node(node_ptr p);

    // 初始化
    void fill_init( size_type n,const value_type& value);

    template<class Iter>
    void copy_init(Iter first,Iter last);

    // 连接/解除
    iterator link_iter_node(const_iterator pos,base_ptr node);
    void link_nodes(base_ptr p,base_ptr first,base_ptr last);
    void link_nodes_at_front(base_ptr first,base_ptr last);
    void link_nodes_at_back(base_ptr first,base_ptr last);
    void unlink_nodes(base_ptr f,base_ptr l);

    // 分配
    void fill_assign( size_type n,const value_type& value);
    template<class Iter>
    void copy_assign(Iter first,Iter last);

    // 插入
    iterator fill_insert(const_iterator pos,size_type n,const value_type& value);

    template<class Iter>
    iterator copy_insert(const_iterator pos,size_type n,Iter first);
    
    // sort 
    template<class Compared>
    iterator list_sort(iterator first,iterator last,size_type n,Compared comp);

};
}
#endif //MYTINYSTL_LIST_H
