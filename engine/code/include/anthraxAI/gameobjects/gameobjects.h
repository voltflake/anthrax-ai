#pragma once

#include <atomic>
#include <iostream>
#include <vector>
#include <map>

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"

namespace Keeper {

    struct Info {
        int ID = 0;
        Vector3<float> Position;
        std::string Material;
        std::string Vertex;
        std::string Fragment;
        std::string Texture;
        std::string Model;
        bool IsModel = false;
        
        inline static std::atomic_int ObjectCounter = 1;

        Info() { ID = ObjectCounter; ObjectCounter++; }
    };  

    enum Type {
        CAMERA = 0,
        SPRITE,
        NPC,
        TEST,
        SIZE
    };
    
    class Objects
    {
        public:
            Objects() {}
            Objects(const Info& info);
            virtual ~Objects() {}
            
            virtual std::string GetModelName() const { return ""; }
            virtual std::string GetTextureName() const { return ""; }
            virtual std::string GetFragmentName() const { return ""; }
            virtual std::string GetVertexName() const { return ""; }
            virtual std::string GetMaterialName() const { return ""; }
            virtual Vector3<float> GetPosition() const { return {0.0f, 0.0f, 0.0f}; }

            virtual Type GetType() { return SIZE; }
            virtual void Update() {}
            virtual void PrintInfo() {}
    };
    
    typedef std::map<int, std::vector<Objects*>> GameObjectsMap;
    
    class Base
    {
        public:
            ~Base();
            void CleanIfNot(Keeper::Type type);
    
            template<typename T>
            void Create(T* type) { ASSERT(type->GetType() == SIZE, "Keeper::Base::Create(): Error: child has no GetType() defined"); ObjectsList[type->GetType()].push_back(type); }
    
            template<typename T>
            T* Get(Type type) { if (!ObjectsList[type].empty()) { return dynamic_cast<T*>(ObjectsList[type][0]); } return nullptr; }
            void Create(const std::vector<Info>& info); 
            void Update();
           
            std::vector<Objects*> GetObjects(Type type) { return (ObjectsList[type]); }
            const GameObjectsMap& GetObjects() const { return ObjectsList; }
                   
        private:
            GameObjectsMap ObjectsList;
    };
}
