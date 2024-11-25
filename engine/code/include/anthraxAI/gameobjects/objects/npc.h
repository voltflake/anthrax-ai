#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"

namespace Keeper 
{
  class Npc : public Objects 
  {
        public:
            Npc() {}
            Npc(const Info& info);
            
            ~Npc() {}
            
            Type GetType() override { return ObjectType; }
            void Update() override { } 
            void PrintInfo() override;

            std::string GetModelName() const override { return ModelName; }
            std::string GetTextureName() const override { return TextureName; }
            std::string GetMaterialName() const override { return MaterialName; }
            std::string GetFragmentName() const override { return Fragment; }
            std::string GetVertexName() const override { return Vertex; }
        private:
            Keeper::Type ObjectType = Type::NPC;
            
            Vector3<float> Position;
           
            std::string Vertex; 
            std::string Fragment;
            std::string TextureName;
            std::string MaterialName;
            std::string ModelName;
            bool IsVisible = false;
  };
}

