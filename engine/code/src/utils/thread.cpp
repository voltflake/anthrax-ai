#include "anthraxAI/utils/thread.h"
#include <queue>
#include <thread>
#include <utility>

void Thread::Pool::Process(const Task& task)
{
    switch (task.type) {
        case Thread::Task::Type::EXECUTE: {
            if (task.name == Thread::Task::Name::RENDER) {
                task.func1();
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

void Thread::Pool::Wait()
{
    for (int i = 0; i < RenderThreads.size(); i++) {
        std::unique_lock<std::mutex> lock(RenderMutex[i]);
        RenderCondition[i].wait(lock, [this, i]{ return RenderQueue[i].empty(); });
    }
}

void Thread::Pool::WorkRender(int id)
{
    while (!Done) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(RenderMutex[id]);
            std::queue<Task>& queue = RenderQueue[id];
            RenderCondition[id].wait(lock, [this, &id]{ return !RenderQueue[id].empty() || Done; });
            if (Done) {
                break;
            }
            task = RenderQueue[id].front();
           // RenderQueue.pop();
        }
        //if (!OnPause) {
            Process(task);
    {
        std::lock_guard<std::mutex> lock(RenderMutex[id]);
		RenderQueue[id].pop();
		RenderCondition[id].notify_one();
        }
            //}
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

    printf("MAX THREADS = %d \n", std::thread::hardware_concurrency());
    RenderQueue.resize(MAX_THREAD_NUM);
    RenderThreads.reserve(MAX_THREAD_NUM);
    for (int i = 0; i < MAX_THREAD_NUM; ++i) {
        RenderThreads.emplace_back(std::thread(&Thread::Pool::WorkRender, this, i));
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
    int i = 0;
    for (std::thread& thread : RenderThreads) {
        RenderCondition[i].notify_all();
        thread.join();
        i++;
    }

    Done = false;
    Threads.clear();
    RenderThreads.clear();
    RenderQueue.clear();
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
    int i = 0;
    for (std::thread& thread : RenderThreads) {
        RenderCondition[i].notify_all();
        if (thread.joinable()) {
            thread.join();
        }
        i++;
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

bool Thread::Pool::PushByID(int id, const Task& task)
{
    if (RenderThreads.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(RenderMutex[id]);

    RenderQueue[id].push(task);
    RenderCondition[id].notify_all();

    return true;
}
