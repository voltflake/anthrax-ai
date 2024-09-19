#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"

VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = flags;
    return fenceCreateInfo;
}

VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo semCreateInfo = {};
    semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semCreateInfo.pNext = nullptr;
    semCreateInfo.flags = flags;
    return semCreateInfo;
}

VkCommandBufferBeginInfo CmdBeginInfo(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;

    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

VkSubmitInfo SubmitInfo(VkCommandBuffer* cmd)
{
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;

    info.waitSemaphoreCount = 0;
    info.pWaitSemaphores = nullptr;
    info.pWaitDstStageMask = nullptr;
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd;
    info.signalSemaphoreCount = 0;
    info.pSignalSemaphores = nullptr;

    return info;
}

void Gfx::Renderer::Sync()
{
	VkFenceCreateInfo fencecreateinfo = FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semcreateinfo = SemaphoreCreateInfo(0);
	
	VkFenceCreateInfo uploadfencecreateinfo = FenceCreateInfo(0);
	VK_ASSERT(vkCreateFence(Gfx::Device::GetInstance()->GetDevice(), &uploadfencecreateinfo, nullptr, &Upload.UploadFence), "failder to create upload fence ! ");
	Core::Deletor::GetInstance()->Push([=, this]() {
		vkDestroyFence(Gfx::Device::GetInstance()->GetDevice(), Upload.UploadFence, nullptr);
	});
	for (int i = 0; i < MAX_FRAMES; i++) {
		VK_ASSERT(vkCreateFence(Gfx::Device::GetInstance()->GetDevice(), &fencecreateinfo, nullptr, &Frames[i].RenderFence), "failder to create fence ! ");
	
		VK_ASSERT(vkCreateSemaphore(Gfx::Device::GetInstance()->GetDevice(), &semcreateinfo, nullptr, &Frames[i].PresentSemaphore), "failder to create present semaphore!");
		VK_ASSERT(vkCreateSemaphore(Gfx::Device::GetInstance()->GetDevice(), &semcreateinfo, nullptr, &Frames[i].RenderSemaphore), "failder to create render semaphore!");

		Core::Deletor::GetInstance()->Push([=, this]() {
			vkDestroyFence(Gfx::Device::GetInstance()->GetDevice(), Frames[i].RenderFence, nullptr);
			vkDestroySemaphore(Gfx::Device::GetInstance()->GetDevice(), Frames[i].PresentSemaphore, nullptr);
			vkDestroySemaphore(Gfx::Device::GetInstance()->GetDevice(), Frames[i].RenderSemaphore, nullptr);
		});
	}
}

void Gfx::Renderer::CleanResources()
{
	for (auto& list : Textures) {
        vkDestroySampler(Gfx::Device::GetInstance()->GetDevice(), *list.second.GetSampler(), nullptr);
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), list.second.GetImageView(), nullptr);
        vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), list.second.GetImage(), nullptr);
	    vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), list.second.GetDeviceMemory(), nullptr);
    }
    Textures.clear();

	if (DepthRT) {
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetDeviceMemory(), nullptr);
		delete DepthRT;
	}
	if (MainRT) {
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetDeviceMemory(), nullptr);
		delete MainRT;
	}
}

void Gfx::Renderer::CreateRenderTargets()
{
	if (DepthRT) {
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), DepthRT->GetDeviceMemory(), nullptr);
		delete DepthRT;
	}
	DepthRT = new RenderTarget();
	DepthRT->SetFormat(VK_FORMAT_D32_SFLOAT);
	DepthRT->SetDepth(true);
	DepthRT->SetDimensions(Core::WindowManager::GetInstance()->GetScreenResolution());
	DepthRT->CreateRenderTarget();
	
	if (MainRT) {
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), MainRT->GetDeviceMemory(), nullptr);
		delete MainRT;
	}
	MainRT = new RenderTarget(*DepthRT);
	MainRT->SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT);
	MainRT->SetDepth(false);
	MainRT->CreateRenderTarget();
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

	//VkCommandPoolCreateInfo uploadcommandpoolinfo = commandpoolcreateinfo(queuefamilyindices.graphicsfamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VK_ASSERT(vkCreateCommandPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &Upload.CommandPool), "failed to create upload command pool!");
	Core::Deletor::GetInstance()->Push([=, this]() {
		vkDestroyCommandPool(Gfx::Device::GetInstance()->GetDevice(), Upload.CommandPool, nullptr);
	});
	VkCommandBufferAllocateInfo cmdallocinfo = CommandBufferCreateInfo(Upload.CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VK_ASSERT(vkAllocateCommandBuffers(Gfx::Device::GetInstance()->GetDevice(), &cmdallocinfo, &Upload.CommandBuffer), "failed to allocate upload command buffers!");
}

void Gfx::Renderer::Submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VkCommandBuffer cmd = Upload.CommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = CmdBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdBeginInfo), "failed to begin command buffer!");
	function(cmd);
	VK_ASSERT(vkEndCommandBuffer(cmd), "failed to end command buffer!");

	VkSubmitInfo submitinfo = SubmitInfo(&cmd);
	VK_ASSERT(vkQueueSubmit(Gfx::Device::GetInstance()->GetQueue(GRAPHICS_QUEUE), 1, &submitinfo, Upload.UploadFence), "failed to submit upload queue!");

	vkWaitForFences(Gfx::Device::GetInstance()->GetDevice(), 1, &Upload.UploadFence, true, 9999999999);
	vkResetFences(Gfx::Device::GetInstance()->GetDevice(), 1, &Upload.UploadFence);
	vkResetCommandPool(Gfx::Device::GetInstance()->GetDevice(), Upload.CommandPool, 0);
}
