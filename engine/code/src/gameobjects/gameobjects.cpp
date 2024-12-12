#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"
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

void Keeper::Base::CleanIfNot(Keeper::Type type)
{
  for (auto& it : ObjectsList) {
    if (it.first != type) {
      for (auto* obj : it.second) {
      if (obj) {
        delete obj;
      }
    }
    it.second.clear();
    }
  }
}

Keeper::Base::Base()
{
  Keeper::Info info;
  info.Fragment = "model.frag";
  info.Vertex = "model.vert";
  info.IsModel = true;
  info.Model = "axis.obj";
  info.Material = "models";
  info.Position = {0.0};
  info.Texture = "dummy.png";

  GizmoInfo[Keeper::Gizmo::Type::X] = info;
}

void Keeper::Base::Update()
{
    int id = SelectedID;
    std::vector<Objects*>::iterator selected_it = std::find_if(ObjectsList[Keeper::Type::NPC].begin(), ObjectsList[Keeper::Type::NPC].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id; });

    for (auto& it : ObjectsList) {
        if (it.first == Keeper::Type::GIZMO) continue;
        for (Keeper::Objects* obj : it.second) {
            obj->SetSelected(obj->GetID() == SelectedID);
            obj->Update();
        }
    }
    for (Keeper::Objects* obj : ObjectsList[Keeper::Type::GIZMO]) {
      //  obj->SetVisible(false);

        if (selected_it != ObjectsList[Keeper::Type::NPC].end()) {
            obj->SetVisible(true);
            // obj->SetPosition(selected_it->GetPosition());
            // obj->SetHandle(SelectedID);
        }
        else {
            obj->SetVisible(false);
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
