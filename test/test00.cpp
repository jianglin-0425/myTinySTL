#include <iostream>

enum
{
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256
};

// 根据申请的字节数 bytes，返回对应的 free-list 索引
// free-list 共 64 档，每档负责固定大小的内存块
static size_t freelist_index(size_t bytes)
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

int main(){
    int bytes = 100;
    std::cout<<"bytes: "<<bytes<<"\t index="<<freelist_index(bytes)<<std::endl;

    return 0;
}