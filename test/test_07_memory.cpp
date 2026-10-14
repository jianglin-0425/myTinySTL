#include <cstdlib>
#include <cstdio>
#include <climits>
#include <utility>
#include <cstddef>

// 模拟：只允许分配 <= 400 字节
void* my_malloc(size_t n) {
    if (n > 400) return nullptr;
    return std::malloc(n);
}

template <class T>
std::pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
        len = INT_MAX / sizeof(T);
    while (len > 0) {
        T* tmp = static_cast<T*>(my_malloc(static_cast<size_t>(len) * sizeof(T)));
        if (tmp)
            return std::pair<T*, ptrdiff_t>(tmp, len);
        len /= 2;
    }
    return std::pair<T*, ptrdiff_t>(nullptr, 0);
}

int main() {
    // 请求 200 个 int（800 字节），但系统只允许 <= 400 字节
    ptrdiff_t want = 200;
    auto result = get_buffer_helper(want, (int*)nullptr);
    int* buf = result.first;
    ptrdiff_t actual = result.second;

    printf("请求 %td 个 int，实际 %td 个\n", want, actual);

    // 正确用法：检查 actual
    if (actual < want) {
        printf("警告：只拿到 %td 个，少于请求的 %td 个\n", actual, want);
        // 选择 1：用 actual 个
        // 选择 2：报错
        // 选择 3：分多次拿
    }

    // 假设选择 1：用 actual 个
    if (buf) {
        for (ptrdiff_t i = 0; i < actual; ++i)
            buf[i] = static_cast<int>(i);
        printf("成功填充 %td 个元素\n", actual);
        std::free(buf);
    }
}