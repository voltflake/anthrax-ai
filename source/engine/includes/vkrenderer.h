#pragma once

#include "vkdefines.h"
#include "vkdevices.h"

class RenderBuilder {
public:
	void 							init(DeviceBuilder& device, DeletionQueue& deletor) 
									{ devicehandler = device; deletorhandler = deletor; };

	void 							buildcommandpool();
	VkCommandPool& 					getcommandpool() 		{ return commandpool;};
	VkCommandBuffer& 				getcommandbuffers() 	{ return commandbuffers;};
	VkRenderPass& 					getrenderpass() 		{ return renderpass;};
	std::vector<VkFramebuffer>& 	getframebuffers() 		{ return framebuffers;};
	VkSemaphore& 					getrendersemaphore() 	{ return rendersemaphore; };
	VkSemaphore& 					getpresentsemaphore() 	{ return presentsemaphore; };
	VkFence& 						getrenderfence()		{ return renderfence; };

	FrameArray& 					getframedata()			{ return frames; };
	DeviceBuilder&					getdevice() 			{ return devicehandler ;};
	
	void 							buildrenderpass();
	void 							builframebuffers();
	void 							sync();

	void 							immediatesubmit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	DeletionQueue					deletorhandler;
	DeviceBuilder					devicehandler;

	VkCommandPool					commandpool;
	VkCommandBuffer					commandbuffers;
	VkRenderPass 					renderpass;
	std::vector<VkFramebuffer> 		framebuffers;
	VkSemaphore 					presentsemaphore, rendersemaphore;
	VkFence 						renderfence;
	FrameArray  					frames;

	UploadContext 					uploadcontext;

	VkFenceCreateInfo 				fencecreateinfo(VkFenceCreateFlags flags = 0);
	VkSemaphoreCreateInfo 			semaphorecreateinfo(VkSemaphoreCreateFlags flags = 0);

	VkCommandPoolCreateInfo 		commandpoolcreateinfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBufferAllocateInfo 	commandbufferallcoteinfo(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBufferBeginInfo 		commandbufferbegininfo(VkCommandBufferUsageFlags flags = 0);
	VkSubmitInfo 					submitinfo(VkCommandBuffer* cmd);
};