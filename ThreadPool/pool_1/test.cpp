#include "ThreadPool.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

void taskFunc(void *arg)
{
    int nNum = *(int *)arg;
    cout << "thread: " << std::this_thread::get_id() << ", number=" << nNum << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
int main()
{
    // 设置线程池最小5个线程，最大10个线程
    ThreadPool pool(5, 10);
    int i;
    // 往任务队列中添加100个任务
    for (i = 0; i < 100; ++i)
    {
        int *pNum = new int(i);
        pool.Add(taskFunc, (void *)pNum);
    }

    cout << "添加任务完成。。。" << endl;
#if 1 //主线程休眠，等待任务全部执行完成
    std::this_thread::sleep_for(std::chrono::seconds(30));
#endif
    return 0;
}
