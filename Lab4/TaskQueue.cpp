#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
    InitializeCriticalSectionAndSpinCount(&criticalSection, SPINS);
    first = nullptr;
    last = nullptr;
}

Task* TaskQueue::Front()
{
    EnterCriticalSection(&criticalSection);
    if (Empty())
    {
        LeaveCriticalSection(&criticalSection);
        return nullptr;
    }
    const Node* queueElement = first;
    if (first == last)
    {
        first = nullptr;
        last = nullptr;
        LeaveCriticalSection(&criticalSection);
        return queueElement->task;
    }
    first = first->next;
    first->prev = nullptr;

    LeaveCriticalSection(&criticalSection);
    Task* task = queueElement->task;
    delete(queueElement);
    return task;
}

void TaskQueue::Enqueue(Task* task)
{
    auto newQueueElement = new Node();
    newQueueElement->task = task;
    newQueueElement->next = nullptr;
    newQueueElement->prev = nullptr;

    EnterCriticalSection(&criticalSection);
    if (Empty())
    {
        first = newQueueElement;
        last = newQueueElement;
    }
    else
    {
        Node* temp = last;
        last->next = newQueueElement;
        newQueueElement->prev = temp;
        last = newQueueElement;
    }
    LeaveCriticalSection(&criticalSection);
}

bool TaskQueue::Empty()
{
    EnterCriticalSection(&criticalSection);
    const bool isEmpty = (first == nullptr) && (last == nullptr);
    LeaveCriticalSection(&criticalSection);
    return isEmpty;
}

TaskQueue::~TaskQueue()
{
    DeleteCriticalSection(&criticalSection);
}

