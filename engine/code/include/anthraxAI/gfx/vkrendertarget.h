#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Gfx
{
    class RenderTarget 
    {
        public:
            RenderTarget() {}
            RenderTarget(uint32_t id) : ID(id) {}
            RenderTarget(const RenderTarget& rt, uint32_t id);

            void CreateRenderTarget();
            void AllocateRTMemory();

	        void MemoryBarrier(VkCommandBuffer cmd, VkImageLayout oldlayout, VkImageLayout newlayout);
	        void Copy(VkCommandBuffer cmd, VkBuffer buffer, uint32_t width, uint32_t height);
            
            VkFormat GetFormat() { return Format; }
            
            void SetFormat(VkFormat format) { Format = format; }
            void SetDimensions(Vector2<int> dim) { Dimensions = dim; }
            void SetDepth(bool depth) { IsDepth = depth; }
            void SetSampler(bool samp) { IsSampler = samp; }

            VkSampler* GetSampler() { return &Sampler; }
            VkImage GetImage() { return Image; }
            VkImageView GetImageView() { return ImageView; }
            VkDeviceMemory GetDeviceMemory() { return Memory; }
            Vector2<int> GetSize() const { return Dimensions; }
            bool IsSamplerSet() const { return IsSampler; }
            bool IsDepthSet() const { return IsDepth; }
            void Clean();

            VkDescriptorSet GetImGuiDescriptor() const { return ImGuiDescriptor; }
            void SetImGuiDescriptor(VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) { ImGuiDescriptor = ImGui_ImplVulkan_AddTexture(Sampler, ImageView, layout); } 

        private:
            VkImage Image;
            VkImageView ImageView;
            VkDeviceMemory Memory;

            VkSampler Sampler;
            VkFormat Format;
            Vector2<int> Dimensions;

            VkDescriptorSet ImGuiDescriptor;

            uint32_t ID = -1; 
            bool IsSampler = false;
            bool IsDepth = false;
            bool IsStorage = false;
    };
}
