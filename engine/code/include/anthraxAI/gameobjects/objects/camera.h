#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/utils/mathdefines.h"

namespace Keeper {
class Camera : public Keeper::Objects 
{
    public:
        enum Type {
            EDITOR = 0,
        };

        Camera() {};
        Camera(Type info, Vector3<float> pos)
            : Type(info), Position(pos.x, pos.y, pos.z) { SetDirections(); }
    

        void SetPosition(glm::vec3 pos) { Position = pos; };
        void SetDirections();

        glm::vec3 GetDir() const { return Direction; }
        glm::vec3 GetPos() const { return Position; }
        glm::vec3 GetFront() const { return Front; }
        glm::vec3 GetUp() const { return Up; }

        void UpdateMovement();

        void UpdateDirection();
        void Update() override { UpdateMovement(); UpdateDirection(); };

        void SetSelected(bool id) override { }
        
        Keeper::Type GetType() const override { return ObjectType; }

    private:
        Keeper::Type ObjectType = Keeper::Type::CAMERA;
        Type Type;

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
