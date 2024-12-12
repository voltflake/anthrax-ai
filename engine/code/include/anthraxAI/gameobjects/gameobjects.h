#pragma once

#include <atomic>
#include <iostream>
#include <vector>
#include <map>

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"

namespace Keeper {

    struct Info {
        Vector3<float> Position;
        std::string Material;
        std::string Vertex;
        std::string Fragment;
        std::string Texture;
        std::string Model;
        bool IsModel = false;
    };

    enum Type {
        CAMERA = 0,
        SPRITE,
        NPC,
        GIZMO,
        TEST,
        SIZE
    };
    
    class Objects
    {
        public:
            Objects() { UniqueID = IDCounter; IDCounter++; }
            Objects(const Info& info) { UniqueID = IDCounter; IDCounter++; }
            virtual ~Objects() {}
            
            virtual std::string GetModelName() const { return ""; }
            virtual std::string GetTextureName() const { return ""; }
            virtual std::string GetFragmentName() const { return ""; }
            virtual std::string GetVertexName() const { return ""; }
            virtual std::string GetMaterialName() const { return ""; }
            virtual Vector3<float> GetPosition() const { return {0.0f, 0.0f, 0.0f}; }

            virtual Type GetType() const { return SIZE; }
            virtual void SetSelected(bool id) { }
            virtual void SetVisible(bool vis) { }
            virtual bool IsVisible() const { return false; }

            virtual void Update() {}

            virtual void PrintInfo() {}

            virtual int GetID() const { return UniqueID; }

        private:
            int UniqueID = 0;
            inline static std::atomic_int IDCounter = 1;
    };
    
    typedef std::map<int, std::vector<Objects*>> GameObjectsMap;
    
    class Base
    {
        public:
            Base();
            ~Base();
            void CleanIfNot(Keeper::Type type);
    
            template<typename T>
            void Create(T* type) { ASSERT(type->GetType() == SIZE, "Keeper::Base::Create(): Error: child has no GetType() defined"); ObjectsList[type->GetType()].push_back(type); }

            std::vector<Objects*>& Get(Type type) { if (!ObjectsList[type].empty()) { return ObjectsList[type]; }  }

            void Create(const std::vector<Info>& info); 
            void Update();
           
            std::vector<Objects*> GetObjects(Type type) { return (ObjectsList[type]); }
            const GameObjectsMap& GetObjects() const { return ObjectsList; }

            Info& GetGizmoInfo(int type) { return GizmoInfo[type]; } ;

            void SetSelectedID(uint32_t id) { SelectedID = id; }
            uint32_t GetSelectedID() { return SelectedID; }
                   
        private:
            GameObjectsMap ObjectsList;
            Info GizmoInfo[3];
            uint32_t SelectedID = 0;
    };
}
