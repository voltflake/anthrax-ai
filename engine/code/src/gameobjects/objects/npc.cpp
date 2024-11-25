#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gfx/vkrenderer.h"
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
}

void Keeper::Npc::PrintInfo()
{
    printf("-------------------------------\n");
    printf("NPC []\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}
