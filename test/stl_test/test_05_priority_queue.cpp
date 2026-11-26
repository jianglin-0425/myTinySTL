#include <queue>
#include "test.h"
#include "../../MyTinySTL/queue.h"




void queue_print(mystl::queue<int> q)
{
    while (!q.empty())
    {
        std::cout << " " << q.front();
        q.pop();
    }
    std::cout << std::endl;
}

void p_queue_print(mystl::priority_queue<int> p)
{
    while (!p.empty())
    {
        std::cout << " " << p.top();
        p.pop();
    }
    std::cout << std::endl;
}

//  queue 的遍历输出
#define QUEUE_COUT(q) do {                       \
    std::string q_name = #q;                     \
    std::cout << " " << q_name << " :";          \
    queue_print(q);                              \
} while(0)

// priority_queue 的遍历输出
#define P_QUEUE_COUT(p) do {                     \
    std::string p_name = #p;                     \
    std::cout << " " << p_name << " :";          \
    p_queue_print(p);                            \
} while(0)

#define QUEUE_FUN_AFTER(con, fun) do {           \
  std::string fun_name = #fun;                   \
  std::cout << " After " << fun_name << " :\n";  \
  fun;                                           \
  QUEUE_COUT(con);                               \
} while(0)

#define P_QUEUE_FUN_AFTER(con, fun) do {         \
  std::string fun_name = #fun;                   \
  std::cout << " After " << fun_name << " :\n";  \
  fun;                                           \
  P_QUEUE_COUT(con);                             \
} while(0)



int main()
{
    std::cout << "[===============================================================]" << std::endl;
    std::cout << "[------------- Run container test : priority_queue -------------]" << std::endl;
    std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
    int a[] = { 1,2,3,4,5} ;

    mystl::vector<int> v1(5);
    mystl::priority_queue<int> p1;
    mystl::priority_queue<int> p2(5);
    mystl::priority_queue<int> p3(5, 1);
    mystl::priority_queue<int> p4(a, a + 5);
    mystl::priority_queue<int> p5(v1);
    mystl::priority_queue<int> p6(std::move(v1));
    mystl::priority_queue<int> p7(p2);
    mystl::priority_queue<int> p8(std::move(p2));
    mystl::priority_queue<int> p9;

    p9 = p3;
    mystl::priority_queue<int> p10;
    p10 = std::move(p3);
    mystl::priority_queue<int> p11{ 1,2,3,4,5 };
    mystl::priority_queue<int> p12;
    p12 = { 1,2,3,4,5 };

    P_QUEUE_FUN_AFTER(p1, p1.push(1));
    P_QUEUE_FUN_AFTER(p1, p1.push(5));
    P_QUEUE_FUN_AFTER(p1, p1.push(3));
    P_QUEUE_FUN_AFTER(p1, p1.pop());
    P_QUEUE_FUN_AFTER(p1, p1.emplace(7));
    P_QUEUE_FUN_AFTER(p1, p1.emplace(2));
    P_QUEUE_FUN_AFTER(p1, p1.emplace(8));
    std::cout << std::boolalpha;
    FUN_VALUE(p1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(p1.size());
    FUN_VALUE(p1.top());
    while (!p1.empty())
    {
        P_QUEUE_FUN_AFTER(p1, p1.pop());
    }
    P_QUEUE_FUN_AFTER(p1, p1.swap(p4));
    P_QUEUE_FUN_AFTER(p1, p1.clear());
    PASSED;
#if PERFORMANCE_TEST_ON
    std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  std::cout << "|         push        |";
#if LARGER_TEST_DATA_ON
  CON_TEST_P1(priority_queue<int>, push, rand(), SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
#else
  CON_TEST_P1(priority_queue<int>, push, rand(), SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
#endif
  std::cout << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  PASSED;
#endif
    std::cout << "[------------- End container test : priority_queue -------------]" << std::endl;
}