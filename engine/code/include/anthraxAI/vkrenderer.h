#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"

namespace QueueHelper {
	VkSemaphoreSubmitInfo semaphoresubmitinfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
	VkCommandBufferSubmitInfo commandbuffersubmitinfo(VkCommandBuffer cmd);
	VkSubmitInfo2 submitinfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);
}

class RenderBuilder {
public:
	void 							init(DeviceBuilder* device, DeletionQueue* deletor) 
									{ devicehandler = device; deletorhandler = deletor; };

	void 							buildcommandpool();
	VkCommandPool& 					getcommandpool() 		{ return commandpool;};
	VkCommandBuffer& 				getcommandbuffers() 	{ return commandbuffers;};
	VkRenderPass& 					getrenderpass() 		{ return renderpass;};
	std::vector<VkFramebuffer>& 	getframebuffers() 		{ return framebuffers;};

	FrameArray& 					getframedata()			{ return frames; };
	DeviceBuilder*					getdevice() 			{ return devicehandler ;};
	
	void 							buildrenderpass();
	void 							builframebuffers(DeviceBuilder& device);
	void							clearframebuffers();
	void 							recreateframebuffer(DeviceBuilder& device);
	void 							sync();

	void 							submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	DeletionQueue*					deletorhandler;
	DeviceBuilder*					devicehandler;

	VkCommandPool					commandpool;
	VkCommandBuffer					commandbuffers;
	VkRenderPass 					renderpass;
	std::vector<VkFramebuffer> 		framebuffers;
	FrameArray  					frames;

	UploadContext 					uploadcontext;

	VkFenceCreateInfo 				fencecreateinfo(VkFenceCreateFlags flags = 0);
	VkSemaphoreCreateInfo 			semaphorecreateinfo(VkSemaphoreCreateFlags flags = 0);

	VkCommandPoolCreateInfo 		commandpoolcreateinfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBufferAllocateInfo 	commandbufferallcoteinfo(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBufferBeginInfo 		commandbufferbegininfo(VkCommandBufferUsageFlags flags = 0);
	VkSubmitInfo 					submitinfo(VkCommandBuffer* cmd);
};