#include "anthraxAI/gameobjects/objects/light.h"
#include "anthraxAI/core/scene.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrenderer.h"

#include <cstdio>
#include <unordered_map>

Keeper::Light::Light(const Info& info)
    : Objects(info)
{
    Position = info.Position;
    ModelName = info.Model;
    TextureName = info.Texture;
    MaterialName = info.Material;
    Vertex = info.Vertex;
    Fragment = info.Fragment;
    ParsedID = info.ParsedID;

    PrintInfo();
}

void Keeper::Light::PrintInfo()
{
    printf("-------------------------------\n");
    printf("NPC [%s]\nPosition [%f, %f, %f]\nMaterial: [%s]\nTexture: [%s]\nModel: [%s]\n", ParsedID.c_str(),
           Position.x, Position.y, Position.z, MaterialName.c_str(), TextureName.c_str(), ModelName.c_str());
    printf("-------------------------------\n");
}

glm::vec3 ProjectMouseLight(float xpos, float ypos, glm::vec3 objpos, int width, int height) {
    float x = 1 -  (2.0f * xpos) / width ;//- 1.0f;
    float y = 1.0f - (2.0f * ypos) / height;
    float z = 1.0f;
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    //printf("---------------\n");
    //printf("MOUSE pos: %f %f\n", xpos, ypos);
    //printf("MOUSE NDS: %f %f %f\n", x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(Gfx::Renderer::GetInstance()->GetProjection() ) * ray_clip;

    //printf("MOUSE EYE: %f %f %f\n", ray_eye.x, ray_eye.y, ray_eye.z);

    glm::vec4 objview = Gfx::Renderer::GetInstance()->GetView() * glm::vec4(objpos, 1.0);
    ray_eye = glm::vec4(ray_eye.x * objview.z, ray_eye.y * objview.z, ray_eye.z * objview.z, ray_eye.w * objpos.z);
    //printf("MOUSE EYE - OBJ VIEW: %f %f %f\n", ray_eye.x, ray_eye.y, ray_eye.z);
    /*printf("Camera POS: %f %f %f\nCamera RIGHT: %f %f %f\nCamera Forward: %f %f %f\nCamera UP: %f %f %f\nRotATION: YAW %f | PITCH: %f\n",   */
    glm::vec4 inv_ray_wor = (glm::inverse( Gfx::Renderer::GetInstance()->GetView()) * (ray_eye));
    glm::vec3 ray_wor = glm::vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
    //printf("RAY WORLD %f %f %f \nOBJ_WORLD %f %f %f\n\n", ray_wor.x, ray_wor.y, ray_wor.z, objpos.x, objpos.y, objpos.z);
    return ray_wor;
}

void Keeper::Light::Update()
{
    if (GizmoHandle && Core::WindowManager::GetInstance()->IsMousePressed() && Selected) {
        Vector2<int> mouse = Core::WindowManager::GetInstance()->GetMousePos();
        Vector2<int> mousebeg = Core::WindowManager::GetInstance()->GetMouseBeginPress();
        Vector2<int> dimensions = Core::WindowManager::GetInstance()->GetScreenResolution();

        glm::vec3 newpos = ProjectMouseLight(mouse.x, mouse.y,glm::vec3(Position.x, Position.y, Position.z), dimensions.x, dimensions.y) ;
        static glm::vec3 pos_on_press2 = glm::vec3(0);
        if (!ResetMouse) {
            ResetMouse = true;
            pos_on_press2 = ProjectMouseLight(mousebeg.x, mousebeg.y,glm::vec3(Position.x, Position.y, Position.z), dimensions.x, dimensions.y) ;
        }

        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Y) {
            Position.y += (newpos.y - pos_on_press2.y);
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::X) {
            Position.x += (newpos.x - pos_on_press2.x  );
            GizmoHandle->SetSelected(true);
        }
        if (GizmoHandle->GetAxis() == Keeper::Gizmo::Type::Z) {
            Position.z -= (newpos.y - pos_on_press2.y);
            GizmoHandle->SetSelected(true);
        }
        pos_on_press2 = newpos;
    }
    else {
        ResetMouse = false;
    }
}
