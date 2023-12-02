#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "TaskQueue.h"

#define SPINS 4000

class ThreadPool
{
public:
    ThreadPool(int threadCount, TaskQueue* taskQueue);
    void WaitAll() const;
    ~ThreadPool();

private:
    HANDLE* threads;
    CRITICAL_SECTION criticalSection;
    int threadsCount;
    static DWORD WINAPI ThreadProc(LPVOID obj);
};

#endif
