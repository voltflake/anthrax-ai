#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include <vulkan/vulkan_core.h>

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
            
            VkFormat GetFormat() { return Format; }
            
            void SetFormat(VkFormat format) { Format = format; }
            void SetDimensions(Vector2<int> dim) { Dimensions = dim; }
            void SetDepth(bool depth) { IsDepth = depth; }

            VkSampler* GetSampler() { return &Sampler; }
            VkImage GetImage() { return Image; }
            VkImageView GetImageView() { return ImageView; }
            VkDeviceMemory GetDeviceMemory() { return Memory; }
            Vector2<int> GetSize() const { return Dimensions; }

            void Clean();

            VkDescriptorSet GetImGuiDescriptor() const { return ImGuiDescriptor; }
            void SetImGuiDescriptor() { ImGuiDescriptor = ImGui_ImplVulkan_AddTexture(Sampler, ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); } 

        private:
            VkImage Image;
            VkImageView ImageView;
            VkDeviceMemory Memory;

            VkSampler Sampler;
            VkFormat Format;
            Vector2<int> Dimensions;

            VkDescriptorSet ImGuiDescriptor;

            bool IsDepth = false;
            bool IsStorage = false;
    };
}
