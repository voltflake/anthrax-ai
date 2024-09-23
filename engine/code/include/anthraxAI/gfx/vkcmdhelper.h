#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"

namespace Gfx
{
    class CommandBuffer
    {
        public:
            VkCommandBuffer GetCmd() { return cmd; }
            void SetCmd(VkCommandBuffer c) { cmd = c; }
            
            VkCommandBufferBeginInfo InfoCmd(VkCommandBufferUsageFlags flags);
            void BeginCmd(VkCommandBufferBeginInfo cmdinfo);
            void EndCmd();

            // VkRenderPassBeginInfo rpinfo(ClearFlags flags, VkRenderPass& rp, VkExtent2D extent, VkFramebuffer framebuffer);
            // void beginrp(VkRenderPassBeginInfo rpinfo, VkSubpassContents contents);
            // void endrp();

            VkSubmitInfo SubmitInfo(VkPipelineStageFlags waitstage, VkSemaphore* presentsem, VkSemaphore* rendersem);
            void Submit(VkQueue queue, VkSubmitInfo subinfo, VkFence* fence);

            VkPresentInfoKHR PresentInfo(VkSwapchainKHR* swapchain, VkSemaphore* rendersem, uint32_t* swapchind);
            VkResult Present(VkQueue queue, VkPresentInfoKHR prinfo);

            // void clearvalues();

        private:
            VkClearValue clearimg[3];
            VkCommandBuffer cmd;
    };
}