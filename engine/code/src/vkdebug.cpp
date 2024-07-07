
#include "anthraxAI/vkdebug.h"

void VkDebug::populateDbgMsgCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; 
}

void VkDebug::setupDebugMessenger(VkInstance vkInstance, VkDebugUtilsMessengerEXT &debugMessenger, bool enableValidationLayers)
{
	if (!enableValidationLayers) 
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDbgMsgCreateInfo(createInfo);

	VK_ASSERT(CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger), "failed to set up debug messenger!");
}

VkResult VkDebug::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
};

void VkDebug::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT &debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebug::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    //if (strcmp(pCallbackData->pMessage, "Validation Error: [ VUID-vkCmdDrawIndexed-subpass-02685 ] Object 0: handle = 0x555557b30640, type = VK_OBJECT_TYPE_COMMAND_BUFFER; Object 1: handle = 0x4e4775000000006e, type = VK_OBJECT_TYPE_PIPELINE; Object 2: handle = 0xd175b40000000013, type = VK_OBJECT_TYPE_RENDER_PASS; | MessageID = 0x38baf2c6 | vkCmdDrawIndexed():  Pipeline was built for subpass 0 but used in subpass 1. The Vulkan spec states: The subpass index of the current render pass must be equal to the subpass member of the VkGraphicsPipelineCreateInfo structure specified when creating the VkPipeline bound to VK_PIPELINE_BIND_POINT_GRAPHICS (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#VUID-vkCmdDrawIndexed-subpass-02685)") != 0)
        //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}