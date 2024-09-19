#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkdebug.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkdescriptors.h"

namespace Gfx
{
    class Vulkan : public Utils::Singleton<Vulkan>
    {
        public:
            void Init();
            void CleanUp();

            VkInstance GetVkInstance() const { return Instance; }
            bool IsValidationLayersOn() const { return ValidationLayersOn; }
        private:
            VkDebug Debug;
            VkInstance Instance;

            void CreateVkInstance();
            bool EnumerateInstanceExtSupport();
            bool EnumerateValidationLayerSupport();

            bool ValidationLayersOn = true;
    };

}