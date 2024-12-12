#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"

namespace Keeper 
{
  class Gizmo : public Objects 
  {
        public:
            enum Type {
              X = 0,
              Y,
              Z,
              SIZE
            };

            Gizmo() {}
            Gizmo(const Info& info);
            
            ~Gizmo() {}
            
            Keeper::Type GetType() const override { return ObjectType; }
            void SetSelected(bool id) override { }
            void SetVisible(bool vis) override { Visible = vis; }
            bool IsVisible() const override{ return Visible; }
            void Update() override { }
            void PrintInfo() override;

            Vector3<float> GetPosition() const override { return Position; }
            std::string GetModelName() const override { return ModelName; }
            std::string GetTextureName() const override { return TextureName; }
            std::string GetMaterialName() const override { return MaterialName; }
            std::string GetFragmentName() const override { return Fragment; }
            std::string GetVertexName() const override { return Vertex; }
        private:
            Keeper::Type ObjectType = Keeper::Type::GIZMO;
            
            Vector3<float> Position;
           
            std::string Vertex; 
            std::string Fragment;
            std::string TextureName;
            std::string MaterialName;
            std::string ModelName;
            bool Visible = false;
  };
}
