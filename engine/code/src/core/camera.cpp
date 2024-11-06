#include "anthraxAI/core/camera.h"
#include "anthraxAI/core/windowmanager.h"
#include <cstdio>

void Core::Camera::SetDirections()
{
    Direction = glm::normalize(Position - Target);
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Right = glm::normalize(glm::cross(WorldUp, Direction)); 
    Up = glm::cross(Direction, Right);
}

void Core::Camera::UpdateMovement()
{
    float delta = Utils::Debug::GetInstance()->DeltaMs;
    const float camspeed = 0.1f * delta;	
    if (Core::WindowManager::GetInstance()->GetPressedKey() == W_KEY) {
        Position += camspeed * Front;
    }
    if (Core::WindowManager::GetInstance()->GetPressedKey() == S_KEY) {
        Position -= camspeed * Front;
    }
    if (Core::WindowManager::GetInstance()->GetPressedKey() == A_KEY) {
        Position -= glm::normalize(glm::cross(Front, Up)) * camspeed;
    }
    if (Core::WindowManager::GetInstance()->GetPressedKey() == D_KEY) {
        Position += glm::normalize(glm::cross(Front, Up)) * camspeed;
    }
}

void Core::Camera::UpdateDirection()
{
    float delta = Utils::Debug::GetInstance()->DeltaMs; 
    Vector2<int> mousemove = Core::WindowManager::GetInstance()->GetMouseDelta();
    Vector2<int> curmousepos = Core::WindowManager::GetInstance()->GetMouseDelta();
    if (curmousepos.x == 0 && curmousepos.y == 0) return;
    float rotspeed = delta * 0.0003f;

    if (mousemove.x || mousemove.y) {
        float yaw = rotspeed * mousemove.x;
        float pitch = rotspeed * mousemove.y;

        Rotation = glm::rotate(glm::mat4(1.0), -yaw, glm::vec3(0.f, 1.f, 0.f));
        Rotation = glm::rotate(Rotation, -pitch, Right);
    }
    Direction = glm::normalize(glm::mat3(Rotation) * Direction);
    Up = glm::normalize(glm::mat3(Rotation) * Up);
    Right = glm::normalize(glm::cross(Direction, Up));
    Front = -Direction;
}
