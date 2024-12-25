#include "anthraxAI/gameobjects/objects/npc.h"
#include <cstdio>

Keeper::Npc::Npc(const Info& info) 
    : Objects(info) 
{
    Position = info.Position;
    ModelName = info.Model;
    TextureName = info.Texture;
    MaterialName = info.Material;
    Vertex = info.Vertex;
    Fragment = info.Fragment;
    Animations = info.Animations;
    IsAnimated = !Animations.empty();
}

void Keeper::Npc::PrintInfo()
{
    printf("-------------------------------\n");
    printf("NPC []\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}

void Keeper::Npc::Update()
{
   if (GizmoHandle) {
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Y) {
            Position.y += 0.5; //GizmoHandle->GetPosition();
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::X) {
            Position.x += 0.5;
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Z) {
            Position.z += 0.5;
        }
    } 
}
