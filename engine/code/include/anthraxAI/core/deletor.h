#pragma once

#include <deque>
#include <functional>
#include "anthraxAI/utils/singleton.h"

namespace Core 
{
    class Deletor : public Utils::Singleton<Deletor>
    {
        std::deque<std::function<void()>> Deletors;

        public:
            void Push(std::function<void()>&& function) {
                Deletors.push_back(function);
            }

            void CleanAll() {
                for (auto it = Deletors.rbegin(); it != Deletors.rend(); it++) {
                    (*it)();
                }
                Deletors.clear();
            }
    };
}