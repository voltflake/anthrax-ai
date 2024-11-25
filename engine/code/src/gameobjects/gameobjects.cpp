#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
Keeper::Base::~Base()
{
  for (auto& it : ObjectsList) {
    for (auto* obj : it.second) {
      if (obj) {
        delete obj;
      }
    } 
  }
  ObjectsList.clear();
}

void Keeper::Base::Update()
{
    for (auto& it : ObjectsList) {
        for (auto* obj : it.second) {
            obj->Update();
        }
    }
}

void Keeper::Base::Create(const std::vector<Keeper::Info>& info)
{
    for (const Keeper::Info& obj : info) {
        if (obj.IsModel) {
           Create<Keeper::Npc>(new Keeper::Npc(obj));
        }
        else {
            Create<Keeper::Sprite>(new Keeper::Sprite(obj));
        }
    } 
}

Keeper::Objects::Objects(const Keeper::Info& info)
     
{

   std::cout << "npc 111 created\n\n";
}
