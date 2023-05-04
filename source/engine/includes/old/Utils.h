#ifndef VULKANVERTEH_H
#define VULKANVERTEH_H

// #define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <D:/Programs/MinGW/glm/glm/glm.hpp>
#include <array>
#include "vulkan\vulkan.h"

struct PlayerVert
{
    glm::vec2 playerPos;
    glm::vec2 oldPos;
};


struct Walls
{
    glm::vec3 rectBegin;
    glm::vec3 rectEnd;

	
	float columnHeightBeg;
	float finColumnBeg;
	float columnHeightEnd;
	float finColumnEnd;

	float wallLenght;

	float hYEnd;
	float wXEnd;
	float wX;
	float hY;

	float rayLength;
	float rayLengthEnd;

	float minAngle;
	float maxAngle;
	float minAngleWall;
	float maxAngleWall;
};

struct Vertex 
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() 
    {
        VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
    {
	    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
	    attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
	    return attributeDescriptions;
	}
};



#endif