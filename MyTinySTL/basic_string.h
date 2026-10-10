//
// Created by user1 on 2025/12/28.
//

#ifndef MYTINYSTL_BASIC_STRING_H
#define MYTINYSTL_BASIC_STRING_H
// 这个头文件包含一个模板类 basic_string
// 用于表示字符串类型

#include <iostream>

namespace mystl
{
    // char_traits
    template <class CharType>
    struct char_traits
    {
        typedef CharType char_type;
        static size_t length(const char_type* str)
        {
            size_t len = 0;
            for (; *str!=char_type(0); ++str) {
                ++len;
            }
            return len;
        }
        static int compare(const char_type* s1,const char_type* s2,size_t n)
        {
            for (;n!=0;--n,++s1,++s2) {
                if(*s1<*s2)
                    return -1;
                if (*s1>*s2)
                    return 1;
            }
            return 0;
        }
        static char_type* copy(char_type* dst,const char_type* src,size_t n)
        {
            MYSTL_DEBUG( src + n < dst || dst + n <= src);
            char_type* r = dst;
            for (;n!=0; --n,++dst,++src) {
                *dst = *src;
            }
            return r;
        }
        static char_type* move(char_type* dst,const char_type* src,size_t n)
        {
            char_type* r = dst;
            if(dst < src)
            {
                for(;n!=0;--n,++dst,++src)
                    *dst= *src;
            }
            else if(src < dst)
            {
                dst += n;
                src += n;
                for(;n!=0;--n)
                    *--dst = *--src;
            }
            return r;
        }
        static char_type* fill(char_type* dst,char_type ch,size_t count)
        {
            char_type* r = dst;
            for (;count>0;--count,++dst) {
                *dst = ch;
            }
            return r;
        }
    };
}

template <>
struct char_traits<char>
{
    typedef char char_type;

    static size_t length(const char_type* str) noexcept
    { return std::strlen(str);}

    static int compare(const char_type* s1,const char_type* s2,size_t n) noexcept
    { return std::memcmp(s1,s2,n);}

    static char_type* copy(char_type* dst,const char_type* src,size_t n) noexcept
    {
        MYSTL_DEBUG(src + n<=dst || dst+n<=src);
        return static_cast<char_type*>(std::memcpy(dst,src,n));
    }

    static char_type* move(char_type* dst,const char_type*src,size_t n) noexcept
    { return static_cast<char_type*>(std::memove(dst,src,n));}

    static char_type* fill(char_type* dst,const char_type ch,size_t count) noexcept
    { return static_cast<char_type*>(std::memset(dst,ch,count));}
};


// char_traits<char32_t>
template <>
struct char_traits<char32_t>
{
    typedef char32_t char_type;
    static size_t length(const char_type* str) noexcept
    {
        size_t len=0;
        for (;*str != char_type(0) ; ++str) {
            ++len;
        }
        return len;
    }

};

// 初始化 basic_string 尝试分配的最小 buffer 大小，可能被忽略
#define STRING_INIT_SIZE 32

// 模板类 basic_string
// 参数一代表字符类型,参数二代表萃取字符类型的方式,缺省使用 mystl::char_traits
template <class CharType,class CharTraits = mystl::char_traits<CharType>>
        class basic_string
        {
        public:
            typedef CharTraits traits_type;
            typedef CharTraits char_traits;

            typedef mystl::allocator<CharType> allocator_type;
            typedef mystl::allocator<CharType> data_allocator;

            typedef typename allocator_type::value_type value_type;
            typedef typename allocator_type::pointer pointer;
            typedef typename allocator_type::const_pointer const_pointer;
            typedef typename allocator_type::reference reference;
            typedef typename allocator_type::const_reference const_reference;
        };


#endif //MYTINYSTL_BASIC_STRING_H
