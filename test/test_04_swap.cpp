#include <iostream>
#include <cstring>
#include <utility>

class MyString{
public:
    char* data_;
    size_t size_;

    MyString(const char *s){
        size_ = std::strlen(s);
        data_ = new char[size_+1];
        std::strcpy(data_,s);
        std::cout << "  [构造] " << data_ << "\n";
    }
    MyString(const MyString& o){        // 拷贝构造
        size_ = o.size_;
        data_ = new char [size_+1];
        std::strcpy(data_,o.data_);
        std::cout << "  [拷贝构造] 分配新内存\n";
    }
    MyString& operator=(const MyString& o){ // 拷贝赋值
        if(this!=&o){
            delete[] data_;
            size_ = o.size_;
            data_ = new char[size_+1];
            std::strcpy(data_, o.data_);
        }
        std::cout << "  [拷贝赋值]\n";
        return *this;
    }

    MyString(MyString&& o) noexcept{ //移动构造
        data_ = o.data_;
        size_ = o.size_;
        o.data_ = nullptr;
        o.size_ = 0;
        std::cout << "  [移动构造] 偷指针\n";
    }
    MyString& operator=(MyString&& o) noexcept{ //移动赋值
        if(this!=&o){
            delete[] data_;
            data_ = o.data_;
            size_ = o.size_;
            o.data_ = nullptr;
            o.size_ = 0;
        }
        std::cout << "  [移动赋值]\n";
        return *this;
    }
    ~MyString(){delete[] data_;}
};

template <class Tp>
void my_swap(Tp& lhs,Tp&rhs){
    auto temp(std::move(lhs));
    lhs = std::move(rhs);
    rhs = std::move(temp);
}
template <class Tp>
void naive_swap(Tp& lhs, Tp& rhs) {
    Tp tmp(lhs);    // 拷贝构造 → 分配内存
    lhs = rhs;      // 拷贝赋值 → 分配内存
    rhs = tmp;      // 拷贝赋值 → 分配内存
}

int main() {
    MyString a("AAA"), b("BBB");
    std::cout << "=== swap 开始 ===\n";
    my_swap(a, b);
    std::cout << "=== swap 结束 ===\n";
    std::cout << "a = " << a.data_ << ", b = " << b.data_ << "\n";

    std::cout<<"\n\n\n";

    std::cout<<"===================================="<<"\n";
    std::cout << "=== swap 开始 ===\n";
    naive_swap(a, b);
    std::cout << "=== swap 结束 ===\n";
    std::cout << "a = " << a.data_ << ", b = " << b.data_ << "\n";


}
