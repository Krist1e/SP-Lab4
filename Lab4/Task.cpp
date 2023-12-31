#include "Task.h"

Task::Task(void (*func)(void*), void* params)
{
    this->func = func;
    this->params = params;
}

Task::~Task()
{
    func = nullptr;
    params = nullptr;
}
