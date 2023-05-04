#ifndef VKFRAMEBUFFER_H
#define VKFRAMEBUFFER_H
// #define VK_USE_PLATFORM_WIN32_KHR

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstring>
#include "vulkan\vulkan.h"

#include "VkTools.h"
#include "Utils.h"

class mFrameBuffer
{
	public:
		void createFramebuffers(std::vector<VkImageView> &swapChainImageViews, std::vector<VkFramebuffer> &swapChainFramebuffers, VkRenderPass renderPass,
	VkExtent2D swapChainExtent, VkDevice device);
		void createCommandPool(VkCommandPool &commandPool, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void createCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, VkCommandPool commandPool,
			VkDevice device, std::vector<VkFramebuffer> &swapChainFramebuffers, VkRenderPass renderPass, VkExtent2D swapChainExtent, VkPipeline &graphicsPipeline,
			VkBuffer &vertexBuffer, VkBuffer &indexBuffer, const std::vector<uint16_t> &indices);
};

#endif