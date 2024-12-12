#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkrendertarget.h"

namespace Keeper 
{
  class Sprite : public Keeper::Objects 
  {
        public:
            Sprite() {}
            Sprite(const Keeper::Info& info);
            
            ~Sprite() {};
            
            Type GetType() const override { return ObjectType; }
            void Update() override { } 
            void SetSelected(bool id) override { }
            void SetVisible(bool vis) override { }
            bool IsVisible() const override{ return Visible; }
            void PrintInfo() override;

            Vector3<float> GetPosition() const override { return Position; }
            std::string GetTextureName() const override { return TextureName; }
            std::string GetMaterialName() const override { return MaterialName; }
            std::string GetFragmentName() const override { return Fragment; }
            std::string GetVertexName() const override { return Vertex; }
        private:
            Keeper::Type ObjectType = Type::SPRITE;
            
            Vector3<float> Position;
           
            std::string Vertex; 
            std::string Fragment;
            std::string TextureName;
            std::string MaterialName;
            std::string MeshName;
            bool Visible = true;
  };
}

