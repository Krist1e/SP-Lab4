#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadCount, TaskQueue* taskQueue)
{
    threads = static_cast<HANDLE*>(malloc(sizeof(HANDLE) * threadCount));
    this->threadsCount = threadCount;
    InitializeCriticalSection(&criticalSection);
    for (int i = 0; i < threadCount; i++)
        threads[i] = CreateThread(nullptr, 0, &ThreadPool::ThreadProc, taskQueue, NULL, nullptr);
}

void ThreadPool::WaitAll() const
{
    WaitForMultipleObjects(threadsCount, threads, TRUE, INFINITE);
}

ThreadPool::~ThreadPool()
{
    DeleteCriticalSection(&criticalSection);
}

DWORD WINAPI ThreadPool::ThreadProc(const LPVOID obj)
{
    auto tasks = static_cast<TaskQueue*>(obj);
    while (true)
    {
        Task* task = tasks->Front();
        if (task == nullptr)
            ExitThread(0);
        task->func(task->params);
    }
}
