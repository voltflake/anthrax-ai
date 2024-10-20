#pragma once
#include "anthraxAI/utils/defines.h"

namespace Core
{
    class Camera 
    {
        public:
            void SetPosition(glm::vec3 pos) { Position = pos; };
            void SetDirections();

            glm::vec3 GetDir() const { return Direction; }
            glm::vec3 GetPos() const { return Position; }
            glm::vec3 GetFront() const { return Front; }
            glm::vec3 GetUp() const { return Up; }

        #ifdef AAI_LINUX
            void UpdateMovement();
        #else
            void UpdateMovement(float delta);
        #endif
            void UpdateDirection();

        private:
            glm::vec3 Position;
            glm::vec3 Target = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 Direction;
            glm::vec3 Right;
            glm::vec3 Front;
            glm::vec3 Up;

            glm::vec3 WorldUp;

            glm::mat4 View;
            glm::mat4 Rotation;

            float Yaw = -90.0f;
            float Pitch = 0.0f;
    };
}