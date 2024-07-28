#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkbuilder.h"

class CmdHandler
{
public:
    VkCommandBuffer get() { return cmd; }
    void set(VkCommandBuffer c) { cmd = c; }
    
    VkCommandBufferBeginInfo cmdinfo(VkCommandBufferUsageFlags flags);
    void begin(VkCommandBufferBeginInfo cmdinfo);
    void end();

    VkRenderPassBeginInfo rpinfo(ClearFlags flags, VkRenderPass& rp, VkExtent2D extent, VkFramebuffer framebuffer);
    void beginrp(VkRenderPassBeginInfo rpinfo, VkSubpassContents contents);
    void endrp();

    VkSubmitInfo submitinfo(VkPipelineStageFlags waitstage, VkSemaphore* presentsem, VkSemaphore* rendersem);
    void submit(VkQueue queue, VkSubmitInfo subinfo, VkFence* fence);

    VkPresentInfoKHR presentinfo(VkSwapchainKHR* swapchain, VkSemaphore* rendersem, uint32_t* swapchind);
    VkResult present(VkQueue queue, VkPresentInfoKHR prinfo);

    void clearvalues();

private:
    VkClearValue clearimg[3];
    VkCommandBuffer cmd;
};