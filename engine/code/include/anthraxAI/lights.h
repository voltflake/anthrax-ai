#pragma once

#include "anthraxAI/vkdefines.h"

enum LightType
{   
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT
};  

class Light
{
    public:
        void init(glm::vec3 pos, glm::vec3 col, LightType t) {
            position = pos;
            color = col;
            type = t;
        }

        glm::vec3 position = glm::vec3(1.0);
        glm::vec3 color = glm::vec3(1.0);

        LightType type;

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
};