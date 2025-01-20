#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"
#include "anthraxAI/core/windowmanager.h"
#include <cstdio>
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

void Keeper::Base::UpdateObjectNames()
{
    if (!ObjectNames.empty()) {
        ObjectNames.clear();
    }
    ObjectNames.reserve(GetObjectsSize());
    for (auto& it : ObjectsList) {
        if (it.first == Keeper::Type::GIZMO) continue;
        for (Keeper::Objects* obj : it.second) {
            std::string objname = "";
            std::string objtype = "";

            if (obj->GetType() == Keeper::Type::CAMERA) {
                objtype = "Camera";
            }
            if (obj->GetType() == Keeper::Type::NPC) {
                objtype = "NPC";
            }
            if (obj->GetType() == Keeper::Type::SPRITE) {
                objtype = "Sprite";
            }
            objname = objtype + ": " + std::to_string(obj->GetID());
            ObjectNames.emplace_back(objname);
        }
    }
    
}

Keeper::Base::Base()
{
    Keeper::Info info;
    info.Fragment = "model.frag";
    info.Vertex = "model.vert";
    info.IsModel = true;
    info.Model = "axisy.obj";
    info.Material = "models";
    info.Position = {0.0};
    info.Texture = "dummy.png";

    GizmoInfo[Keeper::Gizmo::Type::Y] = info;

    info.Model = "axisx.obj";
    GizmoInfo[Keeper::Gizmo::Type::X] = info;
    info.Model = "axisz.obj";
    GizmoInfo[Keeper::Gizmo::Type::Z] = info;
}

void Keeper::Base::Update()
{
    int id = SelectedID;
    std::vector<Objects*>::iterator selected_it = std::find_if(ObjectsList[Keeper::Type::NPC].begin(), ObjectsList[Keeper::Type::NPC].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id; });
    std::vector<Objects*>::iterator gizmo_it = std::find_if(ObjectsList[Keeper::Type::GIZMO].begin(), ObjectsList[Keeper::Type::GIZMO].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id;});
    static bool gizmo = false;
    if (gizmo) {
        gizmo = Core::WindowManager::GetInstance()->IsMousePressed();
    }
    for (Keeper::Objects* obj : ObjectsList[Keeper::Type::GIZMO]) {
        if (selected_it != ObjectsList[Keeper::Type::NPC].end()) {
            obj->SetVisible(true);
            obj->SetHandle(*selected_it);
            obj->SetPosition((*selected_it)->GetPosition());
        }
        if (!gizmo && selected_it == ObjectsList[Keeper::Type::NPC].end() && gizmo_it == ObjectsList[Keeper::Type::GIZMO].end()) {
            obj->SetVisible(false);
            obj->SetHandle(0);
        }
        obj->Update();
    }

    for (auto& it : ObjectsList) {
        if (it.first == Keeper::Type::GIZMO) continue;
        for (Keeper::Objects* obj : it.second) {
            obj->SetSelected(obj->GetID() == SelectedID);
            if (gizmo_it != ObjectsList[Keeper::Type::GIZMO].end()) {
                if ((*gizmo_it)->GetHandle()->GetID() == obj->GetID()) {
                    gizmo = true; 
                    obj->SetGizmo(*gizmo_it);
                    obj->SetSelected(true);
                }
            }
            else if (!gizmo) {
                obj->SetGizmo(nullptr);
            }
            if ((gizmo_it != ObjectsList[Keeper::Type::GIZMO].end() || gizmo) && obj->GetType() == Keeper::Type::CAMERA) {
              continue;
            }
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

size_t Keeper::Base::GetObjectsSize() const
{
    size_t size = 0;
    for (auto& it : ObjectsList) {
        size += it.second.size();
    }
    printf("-----------KEEPER OBJ SIZE: %d\n\n", size);
    return size;
}
