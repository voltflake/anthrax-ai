#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkdefines.h"

namespace Gfx
{
    class RenderTarget 
    {
        public:
            RenderTarget() {}
            RenderTarget(const RenderTarget& rt);

            void CreateRenderTarget();

            VkImage Image;
            VkImageView ImageView;
            VkDeviceMemory Memory;

            VkSampler Sampler;
            VkFormat Format;
            Vector2<int> Dimensions;

            bool IsDepth = false;
            bool IsStorage = false;
    };
}