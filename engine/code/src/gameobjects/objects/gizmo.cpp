#include "anthraxAI/gameobjects/objects/gizmo.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include <cstdio>

Keeper::Gizmo::Gizmo(const Info& info, int axis) 
    : Objects(info) 
{
    Position = info.Position;
    ModelName = info.Model;
    TextureName = info.Texture;
    MaterialName = info.Material;
    Vertex = info.Vertex; 
    Fragment = info.Fragment;
    Axis = static_cast<Gizmo::Type>(axis);
}

void Keeper::Gizmo::SetPosition(const Vector3<float> pos)
{
    Position = pos;
    //if (Axis == Gizmo::Type::Y) {
    Position.y += 3;
    //}
   // if (Axis == Gizmo::Type::X) {
    //    Position.x += 1;
    //}
}

void Keeper::Gizmo::Update()
{
    if (Handle) {
    //    Handle->PrintInfo();
        SetPosition(Handle->GetPosition());
    }
    //if (Handle) {
    //SetPosition(reinterpret_cast<Keeper::Npc*>(Handle)->GetPosition());
    //}
}

void Keeper::Gizmo::PrintInfo()
{
    printf("-------------------------------\n");
    printf("Gizmo []\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}
