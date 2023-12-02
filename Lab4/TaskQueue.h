#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <windows.h>
#include "Task.h"

enum
{
    SPINS = 4000
};

class Node
{
public:
    Task* task;
    Node* prev;
    Node* next;
};

class TaskQueue
{
    Node *first, *last;
    CRITICAL_SECTION criticalSection;

public:
    TaskQueue();
    Task* Front();
    void Enqueue(Task* task);
    bool Empty();
    ~TaskQueue();
};

#endif
