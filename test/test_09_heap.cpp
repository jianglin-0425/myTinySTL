#include <iostream>
#include <vector>
#include <stdexcept>

class MaxHeap
{
private:
    std::vector<int> a_;    //底层数组
    // ========== 核心：上溯 ==========
    // 场景：新元素加到末尾，可能比父节点大
    // 做法：和父节点比，比父大就交换，直到满足堆性质
    void sift_up(int i)
    {
        while (i>0)
        {
            int p = (i-1)/2;    // 父亲节点下标
            if(a_[p] > a_[i])   // 已满足父 >= 子， 停
                break;
            std::swap(a_[p],a_[i]); // 交换位置
            i = p ;             // 继续往上
        }
    }
    // ========== 核心：下沉 ==========
    // 场景：堆顶被换走，可能比孩子小
    // 做法：和较大的孩子比，比孩子小就交换，直到满足堆性质
    // n 是当前有效堆的大小（用于堆排序时缩小范围）
    void sift_down(int i,int n)
    {
        while (true)
        {
            int l = 2*i+1;  // 左孩子
            int r = 2*i+2;  // 右孩子
            int m = i;      // 假设当前最大
            if(l<n && a_[l]>a_[m])
                m = 1;

            if(r<n && a_[r]>a_[m])
                m = r;

            if(m==i)        // 自己最大，停止
                break;

            std::swap(a_[i],a_[m]);
            i = m;          // 继续往下
        }
    }
public:
    MaxHeap() = default;

    // 从数组直接建堆 O(n) 建堆法)
    explicit MaxHeap(const std::vector<int>& arr):a_(arr)
    {
    }
    // ============== 建堆O(n) ============
    // 从最后一个非叶子节点开始，逐个下沉
    void build_heap()
    {
        int n= a_.size();
        for (int i = n/2; i >= 0; --i) {
            sift_down(i,n);
        }

    }

    // ========== 插入 O(log n) ==========
    void push(int v)
    {
        a_.push_back(v);            // 放末尾
        sift_up(a_.size()-1);    // 上浮
    }

    // ========== 取堆顶O(1) ===============
    int top()const{
        if(a_.empty())
            throw std::runtime_error("heap empty");
        return a_[0];
    }

    // ========= 删除栈顶 O(log n )=================
    void pop()
    {
        if(a_.empty())
            throw std::runtime_error("heap empty");
        a_[0] = a_.back();              // 末尾搬到该位置
        a_.pop_back();
        if (!a_.empty())
            sift_down(0,a_.size());
    }

    // ============== 删除任意位置 O(log n) ==============
    void erase(int index)
    {
        if(index <0 || index >= (int)a_.size())
            throw std::runtime_error("index out of range");

        a_[index] = a_.back();              // 末尾搬到该位置
        a_.pop_back();
        if(index < (int)a_.size())
        {
            sift_up(index);             // 可能比父节点大 ——> 上浮
            sift_down(index,a_.size()); // 也可能比孩子节点小 ——> 下沉
        }
    }

    // ========== 修改某位置的值 O(log n) ==========
    void update(int index,int new_val)
    {
        if(index < 0 || index>=(int)a_.size())
            throw std::runtime_error("index out of range");

        int old = a_[index];
        a_[index] = new_val;
        if(new_val>old)
            sift_up(index);     // 变大了 ——> 上浮
        else if(new_val<old)
            sift_down(index,a_.size()); // 变小了 ——> 下沉
    }

    // =============== 堆排序 (升序 O(n log n)) =================
    // 注意；会破坏原堆，排序后 a_ 变成升序数组
    void sort(){
        int n = a_.size();

        // 1. 先建大顶堆 (如果还没建立)
        build_heap();

        // 2. 反复把堆顶 (最大) 换到末尾, 然后缩小堆的范围
        for (int i = n-1; i > 0 ; --i) {
            std::swap(a_[0],a_[i]);     // 最大值放末尾
            sift_down(0,i);              // 在前 i 个 里重新下沉
        }
    }

    // ================= 辅助 ===================
    bool empty() const { return a_.empty();}

    int size() const { return a_.size(); }

    void print() const{
        for(int v: a_)
            std::cout<<v<<" ";

        std::cout<<"\n";
    }

    // 测试： 打印出树形结构
    void print_tree() const{
        int n = a_.size();
        int level = 0,count = 0,per =1;
        for (int i = 0; i < n; ++i) {
            std::cout << a_[i]<<" ";
            if(++count == per){
                std::cout<<" \n";
                ++level;
                count = 0;
                per *= 2;
            }
        }
    }
};

int main() {
    std::cout << "===== 1. 建堆 O(n) =====\n";
    std::vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6};
    MaxHeap h(arr);
    h.print_tree();

    std::cout << "\n===== 2. 插入 O(log n) =====\n";
    h.push(8);
    h.push(7);
    std::cout << "插入 8, 7 后：\n";
    h.print_tree();

    std::cout << "\n===== 3. 取堆顶 O(1) =====\n";
    std::cout << "top = " << h.top() << "\n";

    std::cout << "\n===== 4. 删除堆顶 O(log n) =====\n";
    h.pop();
    std::cout << "pop 后：\n";
    h.print_tree();

    std::cout << "\n===== 5. 删除任意位置 =====\n";
    h.print();
    h.erase(2);        // 删除下标 2
    std::cout << "erase(2) 后：\n";
    h.print_tree();

    std::cout << "\n===== 6. 修改某位置的值 =====\n";
    h.update(1, 100);  // 下标 1 改成 100
    std::cout << "update(1, 100) 后：\n";
    h.print_tree();

    std::cout << "\n===== 7. 堆排序（升序）=====\n";
    MaxHeap h2({5, 2, 8, 1, 9, 3, 7, 4, 6});
    std::cout << "排序前：";
    h2.print();
    h2.sort();
    std::cout << "排序后：";
    h2.print();

    return 0;
}