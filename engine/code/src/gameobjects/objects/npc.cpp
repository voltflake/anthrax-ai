#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "glm/geometric.hpp"

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
    ParsedID = info.ParsedID;

    PrintInfo();
}

void Keeper::Npc::PrintInfo()
{
    printf("-------------------------------\n");
    printf("NPC [%s]\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", ParsedID.c_str(), 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}


glm::vec3 ProjectMouse(float xpos, float ypos, int width, int height) {
    float x = (2.0f * xpos) / width - 1.0f;
    float y = (2.0f * ypos) / height - 1.0; 
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(Gfx::Renderer::GetInstance()->GetProjection() ) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0f);
    glm::vec4 inv_ray_wor = (glm::inverse(Gfx::Renderer::GetInstance()->GetView()) * ray_eye);
    glm::vec3 ray_wor = glm::vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
    ray_wor = glm::normalize(ray_wor);
    return ray_wor;
}

void Keeper::Npc::Update()
{
   if (GizmoHandle && Core::WindowManager::GetInstance()->IsMousePressed() && Selected) {
        Vector2<int> mouse = Core::WindowManager::GetInstance()->GetMousePos();
        Vector2<int> dimensions = Core::WindowManager::GetInstance()->GetScreenResolution();
        
        glm::vec3 newpos = ProjectMouse(mouse.x, mouse.y, dimensions.x, dimensions.y) ;
        float t = 1.0;
        glm::vec3 worldpos =glm::normalize(Gfx::Renderer::GetInstance()->GetCameraPos()) + t * newpos;
        
        float length = glm::length(glm::normalize(glm::vec3(Position.x, Position.y, Position.z) - Gfx::Renderer::GetInstance()->GetCameraPos()));
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Y) {
            Position.y += worldpos.y * length;
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::X) {
            Position.x += worldpos.x * length;
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Z) {
            Position.z += (worldpos.y / worldpos.x) * length;
            GizmoHandle->SetSelected(true);
        }
    }
}
