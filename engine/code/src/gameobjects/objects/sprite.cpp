#include "anthraxAI/gameobjects/objects/sprite.h"

Keeper::Sprite::Sprite(const Info& info) 
    : Objects(info) 
{
    Position = info.Position;
    MeshName = info.Texture;
    TextureName = info.Texture;
    MaterialName = info.Material;
    Vertex = info.Vertex; 
    Fragment = info.Fragment;
}


void Keeper::Sprite::PrintInfo()
{
    printf("-------------------------------\n");
    printf("Sprite []\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nMesh: [%s]\n", 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), MeshName.c_str());
    printf("-------------------------------\n");
}
