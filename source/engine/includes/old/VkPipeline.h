#ifndef VKPIPELINE_H
#define VKPIPELINE_H

// #define VK_USE_PLATFORM_WIN32_KHR

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include "vulkan\vulkan.h"
#include "Utils.h"


class mPipeline
{
	public:
		void createRenderPass(VkFormat swapChainImageFormat, VkRenderPass &renderPass, VkDevice device);
		void createGraphicsPipeline( VkDevice device, VkExtent2D swapChainExtent,
			VkPipelineLayout &pipelineLayout, VkRenderPass &renderPass, VkPipeline &graphicsPipeline);
		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);
};

#endif