#ifndef VULKANTOOLS_H
#define VULKANTOOLS_H

// #define VK_USE_PLATFORM_WIN32_KHR

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <cstring>
#include "vulkan\vulkan.h"
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

// const std::vector<const char*> instanceExtensionsNames = 
// {VK_KHR_SURFACE_EXTENSION_NAME, /*VK_KHR_WIN32_SURFACE_EXTENSION_NAME,*/ VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

namespace vkTools
{
	struct Queue
	{
		VkQueue graphicsQueue;
		VkQueue presentQueue; 
	};

	struct QueueFamilyIndex 
	{
    	std::optional<uint32_t> graphicsFamily;
    	std::optional<uint32_t> presentFamily;

    	bool isComplete() {
    		return graphicsFamily.has_value() && presentFamily.has_value();
    	}
	};

	struct SwapChainSupportDetails 
	{
	    VkSurfaceCapabilitiesKHR capabilities;
	    std::vector<VkSurfaceFormatKHR> formats;
	    std::vector<VkPresentModeKHR> presentModes;
	};

	void pickPhysicalDevice(VkInstance vkInstance, VkPhysicalDevice &physicalDevice, VkSurfaceKHR surface);
	void createLogicalDevice(VkDevice &device, VkPhysicalDevice &physicalDevice, VkSurfaceKHR surface, Queue &mQueue);
	QueueFamilyIndex findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

	void createSwapChain(VkPhysicalDevice &physicalDevice, VkSurfaceKHR surface, VkDevice device,
	VkSwapchainKHR &swapChain, HWND hWnd, std::vector<VkImage> &swapChainImages, VkFormat &swapChainImageFormat, VkExtent2D &swapChainExtent);
	void createImageViews(std::vector<VkImage> &swapChainImages, std::vector<VkImageView> &swapChainImageViews, VkDevice device, VkFormat &swapChainImageFormat);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) ;
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, HWND hWnd);

	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
	bool checkInstanceExtensionsSupported();
	bool checkValidationLayerSupport();
}

#endif