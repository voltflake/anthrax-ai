#pragma once
#include "anthraxAI/vkdefines.h"

namespace VkDebug
{
	void setupDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT &debugMessenger, bool enableValidationLayers);
	void populateDbgMsgCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		    VkDebugUtilsMessageTypeFlagsEXT messageType,
		    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		    void* pUserData);


	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) ;

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT &debugMessenger, const VkAllocationCallbacks* pAllocator) ;

}