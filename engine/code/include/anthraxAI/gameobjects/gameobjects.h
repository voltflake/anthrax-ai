#pragma once

#include <atomic>
#include <iostream>
#include <string>
#include <sys/types.h>
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
        std::vector<std::string> Animations;
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
            Objects(const Info& info, int axis) { UniqueID = IDCounter; IDCounter++; }
            virtual ~Objects() {}
            
            virtual Type GetType() const { return SIZE; }
            
            virtual std::string GetModelName() const { return ""; }
            virtual std::string GetTextureName() const { return ""; }
            virtual std::string GetFragmentName() const { return ""; }
            virtual std::string GetVertexName() const { return ""; }
            virtual std::string GetMaterialName() const { return ""; }
            virtual Vector3<float> GetPosition() const { return {0.0f, 0.0f, 0.0f}; }
            virtual const std::vector<std::string>& GetAnimations() const { return EmptyAnimations; }        

            virtual Keeper::Objects* GetHandle() const { return nullptr; } 
            virtual Keeper::Objects* GetGizmo() const { return  nullptr;}
    
            virtual void SetSelected(bool id) { }
            virtual void SetGizmo(Keeper::Objects* gizmo) {}
            virtual void SetPosition(const Vector3<float> pos) { }
            virtual void SetHandle(Keeper::Objects* id) { }
            virtual void SetVisible(bool vis) { }
            virtual bool IsVisible() const { return false; }
            virtual bool HasAnimations() const { return false; }

            virtual void Update() {}

            virtual void PrintInfo() {}

            virtual int GetID() const { return UniqueID; }

        private:
            std::vector<std::string> EmptyAnimations;
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

            bool IsValid(Type type) { if (!ObjectsList[type].empty()) { return true; } return false; }
            std::vector<Objects*>& Get(Type type) {  return ObjectsList[type]; } 

            void Create(const std::vector<Info>& info); 
            void Update();
           
            std::vector<Objects*> GetObjects(Type type) { return (ObjectsList[type]); }
            const GameObjectsMap& GetObjects() const { return ObjectsList; }

            Info& GetGizmoInfo(int type) { return GizmoInfo[type]; } ;

            void SetSelectedID(uint32_t id) { SelectedID = id; }
            uint32_t GetSelectedID() { return SelectedID; }

            size_t GetObjectsSize() const;
            void UpdateObjectNames();
            const std::vector<std::string>& GetObjectNames() const { return ObjectNames; }
                   
        private:
            GameObjectsMap ObjectsList;
            Info GizmoInfo[3];
            uint32_t SelectedID = 0;

            std::vector<std::string> ObjectNames;
    };
}
