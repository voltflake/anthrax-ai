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
            void AllocateRTMemory();

	        void MemoryBarrier(VkCommandBuffer cmd, VkImageLayout oldlayout, VkImageLayout newlayout);
	        void Copy(VkCommandBuffer cmd, VkBuffer buffer, uint32_t width, uint32_t height);
            
            void SetFormat(VkFormat format) { Format = format; }
            void SetDimensions(Vector2<int> dim) { Dimensions = dim; }
            void SetDepth(bool depth) { IsDepth = depth; }

            VkSampler* GetSampler() { return &Sampler; }
            VkImage GetImage() { return Image; }
            VkImageView GetImageView() { return ImageView; }
            VkDeviceMemory GetDeviceMemory() { return Memory; }
            Vector2<int> GetSize() const { return Dimensions; }

        private:
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