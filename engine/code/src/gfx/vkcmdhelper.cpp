#include "anthraxAI/gfx/vkcmdhelper.h"


// VkRenderPassBeginInfo Gfx::CommandBuffer::rpinfo(ClearFlags flags, VkRenderPass& rp, VkExtent2D extent, VkFramebuffer framebuffer)
// {
// 	VkRenderPassBeginInfo rpinfo = {};
// 	rpinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
// 	rpinfo.pNext = nullptr;

// 	rpinfo.renderPass = rp;
// 	rpinfo.renderArea.offset.x = 0;
// 	rpinfo.renderArea.offset.y = 0;
// 	rpinfo.renderArea.extent = extent;

// 	rpinfo.framebuffer = framebuffer;

//     rpinfo.clearValueCount = 3;
// 	rpinfo.pClearValues = &clearimg[0];
// 	return rpinfo;
// }

VkCommandBufferBeginInfo Gfx::CommandBuffer::InfoCmd(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo cmdbegininfo = {};
	cmdbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbegininfo.pNext = nullptr;

	cmdbegininfo.pInheritanceInfo = nullptr;
	cmdbegininfo.flags = flags;
    return cmdbegininfo;
}

VkSubmitInfo Gfx::CommandBuffer::SubmitInfo(VkPipelineStageFlags waitstage, VkSemaphore* presentsem, VkSemaphore* rendersem)
{
   	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;
	VkPipelineStageFlags waitstage2 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitstage2;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = presentsem;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = rendersem;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

    return submit;
}

VkPresentInfoKHR Gfx::CommandBuffer::PresentInfo(VkSwapchainKHR* swapchain, VkSemaphore* rendersem, uint32_t* swapchind)
{
    VkPresentInfoKHR presentinfo = {};
	presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentinfo.pNext = nullptr;
	presentinfo.pSwapchains = swapchain;
	presentinfo.swapchainCount = 1;
	presentinfo.pWaitSemaphores = rendersem;
	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pImageIndices = swapchind;

    return presentinfo;
}

// void Gfx::CommandBuffer::clearvalues()
// {
//     VkClearValue clearValue;
// 	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
// 	VkClearValue depthClear;
// 	depthClear.depthStencil = {1.0f, 0};

// 	clearimg[0] = clearValue;
//     clearimg[1] = clearValue;
//     clearimg[2] = depthClear;
// }

VkResult Gfx::CommandBuffer::Present(VkQueue queue, VkPresentInfoKHR prinfo)
{
    return vkQueuePresentKHR(queue, &prinfo);
}

void Gfx::CommandBuffer::Submit(VkQueue queue, VkSubmitInfo subinfo, VkFence* fence)
{
	VK_ASSERT(vkQueueSubmit(queue, 1, &subinfo, *fence), "failed to submit queue!");
}

void Gfx::CommandBuffer::BeginCmd(VkCommandBufferBeginInfo cmdinfo)
{
	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdinfo), "failed to begin a command buffer!");
}

void Gfx::CommandBuffer::EndCmd()
{
	VK_ASSERT(vkEndCommandBuffer(cmd), "failder to end command buffer");
}

// void Gfx::CommandBuffer::beginrp(VkRenderPassBeginInfo rpinfo, VkSubpassContents contents)
// {
// 	vkCmdBeginRenderPass(cmd, &rpinfo, contents);
// }

// void Gfx::CommandBuffer::endrp()
// {
// 	vkCmdEndRenderPass(cmd);
// }