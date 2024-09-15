#pragma once

namespace Utils
{
    template <typename T>
    class Singleton 
    {
        private:
        protected:
            Singleton() {}

        public:
            Singleton(const Singleton* obj) = delete;
            Singleton* operator=(const Singleton*) = delete; 

            static T* GetInstance() {  static T Instance; return &Instance; }
    };
}