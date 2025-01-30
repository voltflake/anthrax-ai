#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkdefines.h"

namespace Gfx
{
    struct RenderingAttachmentInfo {
        VkRenderingAttachmentInfoKHR* Info;
        VkImage Image;
        VkImageLayout Layout = VK_IMAGE_LAYOUT_UNDEFINED;
        bool IsDepth;
    };

    class CommandBuffer
    {
        public:
            VkCommandBuffer GetCmd() { return cmd; }
            void SetCmd(VkCommandBuffer c) { cmd = c; }

            VkImageSubresourceRange GetSubresourceMainRange() { return range; }
            
            VkCommandBufferBeginInfo InfoCmd(VkCommandBufferUsageFlags flags);
            void BeginCmd(VkCommandBufferBeginInfo cmdinfo);
            void EndCmd();

            VkSubmitInfo SubmitInfo(VkPipelineStageFlags waitstage, VkSemaphore* presentsem, VkSemaphore* rendersem);
            void Submit(VkQueue queue, VkSubmitInfo subinfo, VkFence* fence);

            VkPresentInfoKHR PresentInfo(VkSwapchainKHR* swapchain, VkSemaphore* rendersem, uint32_t* swapchind);
            VkResult Present(VkQueue queue, VkPresentInfoKHR prinfo);

            // const VkRenderingInfoKHR GetRenderingInfo(VkImage mainimage, VkImage depthimage, VkRenderingAttachmentInfo* color, VkRenderingAttachmentInfo* depth, Vector2<int> extents);
            const VkRenderingInfoKHR GetRenderingInfo(std::vector<RenderingAttachmentInfo>& attachmentinfo, Vector2<int> extents);

            void MemoryBarrier(VkImage image, VkImageLayout oldlayout, VkImageLayout newlayout, VkImageSubresourceRange range);
            void CopyImage(VkImage src, Vector2<int> srcsize, VkImageLayout srcoldlayout, VkImageLayout srcnewlayout, VkImage dst, Vector2<int> dstsize, VkImageLayout dstoldlayout, VkImageLayout dstnewlayout);
        private:
        	VkImageSubresourceRange range{};

            VkClearValue clearimg[3];
            VkCommandBuffer cmd;
    };
}
