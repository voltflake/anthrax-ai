#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/core/windowmanager.h"

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

    PrintInfo();
}

void Keeper::Npc::PrintInfo()
{
    printf("-------------------------------\n");
    printf("NPC []\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", 
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}

glm::vec3 UnprojectMouse(glm::vec2 mouse, glm::vec2 dimensions, glm::vec3 pos) 
{
    float x = (2.0f * mouse.x) / dimensions.x - 1.0f;
    float y = (2.0f * mouse.y) / dimensions.y - 1.0f;
    float z = 1.0f;
    glm::vec4 viewport(0, 0, dimensions.x, dimensions.y);
    glm::vec3 mouse_ndc = { x, y, z };
    
    glm::vec3 realpos = glm::unProject({mouse.x, mouse.y, 1.0}, glm::translate(glm::mat4(1.0f), pos), Gfx::Renderer::GetInstance()->GetProjection(), viewport);

    float delta = Utils::Debug::GetInstance()->DeltaMs;
    float speed = delta * 0.001f;

    return  glm::vec3(realpos.x * speed, realpos.y * speed, 1.0) ;//* glm::vec3(speed);
}

void Keeper::Npc::Update()
{
   if (GizmoHandle && Core::WindowManager::GetInstance()->IsMousePressed()) {
        Vector2<int> mouse = Core::WindowManager::GetInstance()->GetMousePos();
        Vector2<int> dimensions = Core::WindowManager::GetInstance()->GetScreenResolution();

        glm::vec3 newpos = UnprojectMouse({ mouse.x, mouse.y }, { dimensions.x, dimensions.y }, {Position.x, Position.y, Position.z});
       
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Y) {
            Position.y += newpos.y ;
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::X) {
            Position.x += newpos.x ;
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Z) {
            Position.z += newpos.y ;
            GizmoHandle->SetSelected(true);
        }
    } 
}
