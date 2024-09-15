#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"

void Gfx::Renderer::CreateRenderTargets()
{
	if (DepthRT) {
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), DepthRT->ImageView, nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), DepthRT->Image, nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), DepthRT->Memory, nullptr);
		delete DepthRT;
	}
	DepthRT = new RenderTarget();
	DepthRT->Format = VK_FORMAT_D32_SFLOAT;
	DepthRT->IsDepth = true;
	DepthRT->Dimensions = Core::WindowManager::GetInstance()->GetScreenResolution();
	DepthRT->CreateRenderTarget();
	AllocateRTMemory(DepthRT);
	// if (DepthRT) {
	// 	vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), DepthRT->ImageView, nullptr);
	// 	vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), DepthRT->Image, nullptr);
	// 	vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), DepthRT->Memory, nullptr);
	// 	delete DepthRT;
	// }
	// DepthRT = new RenderTarget;
	// DepthRT->Format = VK_FORMAT_D32_SFLOAT;
	// DepthRT->IsDepth = true;
	// DepthRT->Dimensions = Core::WindowManager::GetInstance()->GetScreenResolution();
	// Gfx::RenderTarget::CreateRenderTarget(DepthRT);
}

VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t graphicsfamily, VkCommandPoolCreateFlags flags) {
	
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;

	info.queueFamilyIndex = graphicsfamily;
	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo CommandBufferCreateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level) {
	
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

void Gfx::Renderer::CreateCommands()
{
    Gfx::QueueFamilyIndex indices = Gfx::Device::GetInstance()->FindQueueFimilies(Gfx::Device::GetInstance()->GetPhysicalDevice());

    VkCommandPoolCreateInfo poolinfo = CommandPoolCreateInfo(indices.Graphics.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (int i = 0; i < MAX_FRAMES; i++) {
    	VK_ASSERT(vkCreateCommandPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &Frames[i].CommandPool), "failed to create command pool!");
		
		VkCommandBufferAllocateInfo cmdinfo = CommandBufferCreateInfo(Frames[i].CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VK_ASSERT(vkAllocateCommandBuffers(Gfx::Device::GetInstance()->GetDevice(), &cmdinfo, &Frames[i].MainCommandBuffer), "failed to allocate command buffers!");

		Core::Deletor::GetInstance()->Push([=, this]() {
			vkDestroyCommandPool(Gfx::Device::GetInstance()->GetDevice(), Frames[i].CommandPool, nullptr);
		});
	}
}