#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"

namespace Gfx
{
    class VkDebug
    {
        public:
            VkDebug();

            void Setup(VkInstance instance, bool validationon);
            VkDebugUtilsMessengerCreateInfoEXT* GetInfo() { return &MessengerCreateInfo; }
            void Destroy(VkInstance instance, const VkAllocationCallbacks* pAllocator);
            void SetName(VkDebugUtilsObjectNameInfoEXT info);
        private:
            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) ;
            PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;

            VkDebugUtilsMessengerEXT Messenger;
            VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo;
    };
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
}