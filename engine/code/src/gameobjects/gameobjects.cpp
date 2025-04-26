#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/light.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "glm/common.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include "tracy/Tracy.hpp"
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

void Keeper::Base::CleanIfNot(Keeper::Type type, bool resetID)
{
    for (auto it = ObjectsList.begin(); it != ObjectsList.end(); ) {
        if (it->first != type) {
            for (auto* obj : it->second) {
                if (resetID) {
                    obj->ResetCounterID();
                }
                if (obj) {
                    delete obj;
                }
            }
            it->second.clear();
            ObjectsList.erase(it++);
        }
        else {
            ++it;
        }
    }
}

const Keeper::Objects* Keeper::Base::GetObject(Keeper::Type type, int id) const
{
    std::vector<Keeper::Objects*> vec = ObjectsList.at(type);

    auto it = std::find_if(vec.begin(), vec.end(), [id](Keeper::Objects* o) { return o->GetID() == id; });

    return *it;
}

Keeper::Objects* Keeper::Base::GetNotConstObject(Keeper::Type type, int id)
{
    std::vector<Keeper::Objects*> vec = ObjectsList.at(type);

    auto it = std::find_if(vec.begin(), vec.end(), [id](Keeper::Objects* o) { return o->GetID() == id; });

    return *it;
}
Keeper::Objects* Keeper::Base::GetNotConstObject(Keeper::Type type, const std::string& str)
{
    std::vector<Keeper::Objects*> vec = ObjectsList.at(type);

    auto it = std::find_if(vec.begin(), vec.end(), [str](Keeper::Objects* o) { return o->GetParsedID() == str; });

    return *it;
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
            if (obj->GetType() == Keeper::Type::SPRITE) {
                objtype = "Light";
            }

            std::string def = obj->GetParsedID();
            if (def.empty()) {
                def = std::to_string(obj->GetID());
            }
            objname = objtype + ": " + def;
            ObjectNames.emplace_back(objname);
        }
    }

}

Keeper::Base::Base()
{
    Keeper::Info info;
    info.Fragment = "gizmo.frag";
    info.Vertex = "gizmo.vert";
    info.IsModel = true;
    info.Model = "axisy.obj";
    info.Material = "gizmo";
    info.Position = {0.0};
    info.Texture = "dummy.png";

    GizmoInfo[Keeper::Gizmo::Type::Y] = info;

    info.Model = "axisx.obj";
    GizmoInfo[Keeper::Gizmo::Type::X] = info;
    info.Model = "axisz.obj";
    GizmoInfo[Keeper::Gizmo::Type::Z] = info;

    Keeper::Info modules;
    modules.IsModel = false;
    modules.Position = {0.0f, 0.0f, 0.0f};
    modules.Material = "intro";
    modules.Mesh = "dummy";
    DefaultObjects[Infos::INFO_INTRO] = modules;

    modules.Material = "grid";
    modules.Texture = "dummy.png";
    modules.VertexBase = true;
    modules.Mesh = "";
    DefaultObjects[Infos::INFO_GRID] = modules;

    modules.Material = "outline";
    modules.Texture = "mask";
    modules.Mesh = "dummy.png";
    DefaultObjects[Infos::INFO_OUTLINE] = modules;
    DefaultObjects[Infos::INFO_MASK] = modules;
    modules.VertexBase = false;
    modules.Material = "gbuffer";
    modules.Texture = "albedo";
    modules.Mesh = "dummy.png";
    DefaultObjects[Infos::INFO_GBUFFER] = modules;

    modules.Material = "lighting";
    modules.Texture.clear();
    modules.Textures.reserve(3);
    modules.Textures.push_back("normal");
    modules.Textures.push_back("position");
    modules.Textures.push_back("albedo");
    modules.Mesh = "dummy";
    DefaultObjects[Infos::INFO_LIGHTING] = modules;

}

void Keeper::Base::Update()
{
    int id = SelectedID;
    std::vector<Objects*>::iterator light_it = ObjectsList[Keeper::Type::LIGHT].end();
    std::vector<Objects*>::iterator selected_it = std::find_if(ObjectsList[Keeper::Type::NPC].begin(), ObjectsList[Keeper::Type::NPC].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id; });
    if (selected_it == ObjectsList[Keeper::Type::NPC].end()) {
        light_it = std::find_if(ObjectsList[Keeper::Type::LIGHT].begin(), ObjectsList[Keeper::Type::LIGHT].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id; });
    }
    std::vector<Objects*>::iterator gizmo_it = std::find_if(ObjectsList[Keeper::Type::GIZMO].begin(), ObjectsList[Keeper::Type::GIZMO].end(), [id](const Keeper::Objects* obj) { return obj->GetID() == id;});
    static bool gizmo = false;

    for (Keeper::Objects* obj : ObjectsList[Keeper::Type::GIZMO]) {
        if (selected_it != ObjectsList[Keeper::Type::NPC].end()) {
            obj->SetVisible(true);
            obj->SetHandle(*selected_it);
            obj->SetPosition((*selected_it)->GetPosition());
        }
        if ( light_it != ObjectsList[Keeper::Type::LIGHT].end()) {
            obj->SetVisible(true);
            obj->SetHandle(*light_it);
            obj->SetPosition((*light_it)->GetPosition());
        }
        if ((selected_it == ObjectsList[Keeper::Type::NPC].end() && light_it == ObjectsList[Keeper::Type::LIGHT].end()) && gizmo_it == ObjectsList[Keeper::Type::GIZMO].end()) {
            obj->SetVisible(false);
            obj->SetHandle(0);
        }
        obj->Update();
    }

    for (auto& it : ObjectsList) {
        if (it.first == Keeper::Type::GIZMO) continue;
        if ((gizmo_it != ObjectsList[Keeper::Type::GIZMO].end() || gizmo )&& it.first == Keeper::Type::CAMERA) continue;
        for (Keeper::Objects* obj : it.second) {
            Keeper::Objects* gizmo_handle = nullptr;

            obj->SetSelected(obj->GetID() == SelectedID);
            if (gizmo_it != ObjectsList[Keeper::Type::GIZMO].end() && (*gizmo_it)->GetHandle() && (*gizmo_it)->GetHandle()->GetID() == obj->GetID()) {
                    gizmo_handle = *gizmo_it;
                    obj->SetSelected(true);
            }
            else {
                    obj->SetSelected(false);
                }
            obj->SetGizmo(gizmo_handle);
            obj->Update();
        }
    }
}

void Keeper::Base::SpawnObjects(const Keeper::Info& info)
{
    Vector3<float> offsets = info.Offset;
    ASSERT(offsets.x == 0 && offsets.y == 0 && offsets.z == 0, "Keeper::Base::SpawnObjects(): offsets can't be 0");

    Keeper::Info spawn = info;
    int i = 0;
    for (float x = info.Position.x; x < offsets.x; x += 1.0f ) {
        for (float y = info.Position.y; y < offsets.y; y += 1.0f ) {
            for (float z = info.Position.z; z < offsets.z; z += 1.0f ) {
                spawn.Position = { x, y, z };
                if (!spawn.ParsedID.empty()) {
                    spawn.ParsedID = info.ParsedID + "_" + std::to_string(i);
                    i++;
                }
                Create<Keeper::Npc>(new Keeper::Npc(spawn));
            }
        }
    }
}

void Keeper::Base::Create(const std::vector<Keeper::Info>& info)
{
    for (const Keeper::Info& obj : info) {
        if (obj.IsLight) {
            Create<Keeper::Light>(new Keeper::Light(obj));
        }
        if (obj.IsModel) {
            if (obj.Spawn) {
                SpawnObjects(obj);
            }
            else {
                Create<Keeper::Npc>(new Keeper::Npc(obj));
            }
        }
        else {
            Create<Keeper::Sprite>(new Keeper::Sprite(obj));
        }
    }
}

bool Keeper::Base::Find(Keeper::Type type) const
{
    return ObjectsList.find(type) != ObjectsList.end();
}

size_t Keeper::Base::GetObjectsSize() const
{
    size_t size = 0;
    for (auto& it : ObjectsList) {
        size += it.second.size();
    }
    return size;
}
