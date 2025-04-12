#include "anthraxAI/utils/thread.h"

void Thread::Pool::Process(const Task& task)
{
    switch (task.type) {
        case Thread::Task::Type::EXECUTE: {
            if (task.name == Thread::Task::Name::RENDER) {
               // task.func1(task.obj);
            }
            else {
                if (task.info && task.func) {
                    task.func(task.i, task.info);
                }
            }
            break;
        }
        case Thread::Task::Type::STOP:
            return;
    }
}

void Thread::Pool::Work()
{
    while (!Done) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(Mutex);
            WorkCondition.wait(lock, [&]{ return !Queue.empty() || Done; });
            if (Done) {
                break;
            }
            task = Queue.front();
            Queue.pop();
        }
        if (!OnPause) {
            Process(task);
        }
    }
}

void Thread::Pool::Init(int num)
{
    Threads.reserve(num);
    for (int i = 0; i < num; ++i) {
        Threads.emplace_back(std::thread(&Thread::Pool::Work, this));
    }
}

void Thread::Pool::Reload()
{
    if (Threads.empty() || Done) {
        return;
    }
    Done = true;
    OnPause = false;
    WorkCondition.notify_all();

    for (std::thread& thread : Threads) {
        thread.join();
    }

    Done = false;
    Threads.clear();
    Init(8);
}

void Thread::Pool::Stop()
{
    if (Threads.empty() || Done) {
        return;
    }
    Done = true;
    WorkCondition.notify_all();

    for (std::thread& thread : Threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

bool Thread::Pool::Push(const Task& task)
{
    if (Threads.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(Mutex);

    Queue.push(task);
    WorkCondition.notify_one();
    return true;
}