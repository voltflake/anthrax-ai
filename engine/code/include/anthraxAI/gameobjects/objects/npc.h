#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"
#include "glm/fwd.hpp"

namespace Keeper 
{
  class Npc : public Objects 
  {
        public:
            Npc() {}
            Npc(const Info& info);
            
            ~Npc() {}
            
            Type GetType() const override { return ObjectType; }
            void SetSelected(bool id) override { Selected = id; }
            void SetVisible(bool vis) override { }
            bool IsVisible() const override{ return Visible; }
            void Update() override;
            void PrintInfo() override;
   
            bool HasAnimations() const override { return IsAnimated; }
            void SetGizmo(Keeper::Objects* gizmo) override { GizmoHandle = reinterpret_cast<Keeper::Gizmo*>(gizmo); }
            
            void SetTextureName(const std::string& str) override { TextureName = str; }
           
            Keeper::Objects* GetGizmo() const override { return  reinterpret_cast<Keeper::Objects*>(GizmoHandle);}
            Vector3<float> GetPosition() const override { return Position; }
            std::string GetModelName() const override { return ModelName; }
            std::string GetTextureName() const override { return TextureName; }
            std::string GetMaterialName() const override { return MaterialName; }
            std::string GetFragmentName() const override { return Fragment; }
            std::string GetVertexName() const override { return Vertex; }

            const std::string& GetParsedID() const override { return ParsedID; }
            const std::vector<std::string>& GetAnimations() const override { return Animations; }        
        private:
            Keeper::Type ObjectType = Type::NPC;
            
            Vector3<float> Position;
           
            std::string ParsedID = ""; 
            std::string Vertex; 
            std::string Fragment;
            std::string TextureName;
            std::string MaterialName;
            std::string ModelName;
            bool Visible = true;
            bool Selected = false;
            bool IsAnimated = false;

            std::vector<std::string> Animations;
            
            Keeper::Gizmo* GizmoHandle = nullptr;
  };
}

