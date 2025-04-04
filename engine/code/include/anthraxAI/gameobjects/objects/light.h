#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"

namespace Keeper 
{
  class Light : public Objects 
  {
        public:
            Light() {}
            Light(const Info& info);
            
            ~Light() {}
            
            Type GetType() const override { return ObjectType; }
            void SetSelected(bool id) override { Selected = id; }
            void SetVisible(bool vis) override { }
            bool IsVisible() const override{ return Visible; }
            void Update() override;
            void PrintInfo() override;
   
            void SetGizmo(Keeper::Objects* gizmo) override { GizmoHandle = reinterpret_cast<Keeper::Gizmo*>(gizmo); }
            
            Keeper::Objects* GetGizmo() const override { return  reinterpret_cast<Keeper::Objects*>(GizmoHandle);}
            Vector3<float> GetPosition() const override { return Position; }
            std::string GetModelName() const override { return ModelName; }
            std::string GetTextureName() const override { return TextureName; }
            std::string GetMaterialName() const override { return MaterialName; }
            std::string GetFragmentName() const override { return Fragment; }
            std::string GetVertexName() const override { return Vertex; }

            const std::string& GetParsedID() const override { return ParsedID; }
        private:
            Keeper::Type ObjectType = Type::LIGHT;
            
            Vector3<float> Position;

            bool ResetMouse = false;
           
            std::string ParsedID = ""; 
            std::string Vertex; 
            std::string Fragment;
            std::string TextureName;
            std::string MaterialName;
            std::string ModelName;
            bool Visible = true;
            bool Selected = false;

            bool reset = true;
            
            Keeper::Gizmo* GizmoHandle = nullptr;
  };
}

