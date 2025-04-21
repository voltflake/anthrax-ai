#pragma once

#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/utils/singleton.h"
#include "anthraxAI/gamemodules/modules.h"

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <functional>
#include <thread>
#include <map>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Thread
{
    constexpr uint32_t MAX_THREAD_NUM = 8;

    struct Task {
        enum class Name {
            UPDATE,
            RENDER,
            NONE
        };
        enum class Type {
            EXECUTE,
            STOP,
        };
        Name name;
        Type type;
        std::function<void(int i, Keeper::Objects* info)> func;
        std::function<void()> func1;

        int i;
        Keeper::Objects* info;
        Gfx::RenderObject* obj;
        Gfx::MeshPushConstants* constants;
    };

    class Pool : public Utils::Singleton<Pool>
    {
        public:
            Pool() {}
            ~Pool() { Stop(); }
            void Init(int num);
            bool IsInit() const { return !Threads.empty(); }
            void Stop();
            void Reload();
            bool Push(const Task& func);
            bool PushByID(int id, const Task& func);
            void Pause(bool p) { OnPause = p; } 
            void Wait();
        private:
            std::mutex Mutex;

            std::mutex RenderMutex[MAX_THREAD_NUM];
            std::condition_variable WorkCondition;
            std::condition_variable RenderCondition[MAX_THREAD_NUM];
            bool Done = false;
            bool OnPause = false;

            void Work();
            void WorkRender(int id);
            void Process(const Task& task);
            std::vector<std::thread> Threads;
            std::queue<Task> Queue;
            std::vector<std::queue<Task>> RenderQueue;
            std::queue<Task> RenderQueue2;
            std::vector<std::thread> RenderThreads;
            
            //std::thread RenderThreads2;
    };
    
    static std::map<Task::Name, std::pair<double, double>> Timings;
    static inline void BeginTime(Task::Name name, double i) { Timings[name] = std::make_pair(i, 0); }
    static inline void EndTime(Task::Name name, double i) { Timings[name] = std::make_pair(Timings[name].first, i); }
    static inline void PrintTime(Task::Name name) { 
        std::string str = name == Task::Name::RENDER ? "RENDER" : "UPDATE";
        printf("Task:[%s] worked %lf\n----------\n", str.c_str(), Timings[name].second - Timings[name].first); }

}
