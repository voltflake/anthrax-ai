#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkdescriptors.h"

#include "anthraxAI/core/windowmanager.h"

void Gfx::Renderer::PrepareCameraBuffer()
{
	glm::mat4 view = glm::mat4(1.0f);//glm::lookAt(EditorCamera.getposition(), EditorCamera.getposition() + EditorCamera.getfront(), EditorCamera.getup());
	glm::mat4 projection =  glm::mat4(1.0f);//glm::perspective(glm::radians(45.f), float(Builder.getswapchainextent().width) / float(Builder.getswapchainextent().height), 0.01f, 100.0f);
	projection[1][1] *= -1;

	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;
	camdata.viewpos = glm::vec4(1.0);//glm::vec4(EditorCamera.getposition(), 1.0);
	camdata.mousepos = glm::vec4(1.0);//{Mouse.pos.x, Mouse.pos.y, 0, 0};
	camdata.viewport = { Core::WindowManager::GetInstance()->GetScreenResolution().x ,Core::WindowManager::GetInstance()->GetScreenResolution().y, 0, 0};
//     camdata.dir_light_pos = glm::vec4(DirectionLight.position, 1.0);
//     camdata.dir_light_color = glm::vec4(DirectionLight.color, 1.0);

//     for (int i = 0; i < pointlightamount; i++) {
//         camdata.point_light_pos[i] = glm::vec4(PointLights[i].position, 1.0);
//         camdata.point_light_color[i] = glm::vec4(PointLights[i].color, 1.0);
//     }
//    camdata.pointlightamount = pointlightamount;

	char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES * Gfx::DescriptorsBase::GetInstance()->PadUniformBufferSize(sizeof(CameraData));
  	vkMapMemory(Gfx::Device::GetInstance()->GetDevice(), Gfx::DescriptorsBase::GetInstance()->GetCameraBufferMemory(FrameIndex), 0, sceneParamBufferSize, 0, (void**)&datadst);
   	int frameind = FrameIndex % MAX_FRAMES;
	datadst += Gfx::DescriptorsBase::GetInstance()->PadUniformBufferSize(sizeof(CameraData)) * frameind;
    memcpy( datadst, &camdata, (size_t)sizeof(CameraData));
  	vkUnmapMemory(Gfx::Device::GetInstance()->GetDevice(), Gfx::DescriptorsBase::GetInstance()->GetCameraBufferMemory(FrameIndex));

}

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

uint32_t Gfx::Renderer::SyncFrame()
{
	VK_ASSERT(vkWaitForFences(Gfx::Device::GetInstance()->GetDevice(), 1, &Frames[FrameIndex].RenderFence, true, 1000000000), "vkWaitForFences failed !");
	uint32_t swapchainimageindex;
	VkResult e = vkAcquireNextImageKHR(Gfx::Device::GetInstance()->GetDevice(), Gfx::Device::GetInstance()->GetSwapchain(), 1000000000, Frames[FrameIndex].PresentSemaphore, VK_NULL_HANDLE, &swapchainimageindex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        //winprepared = true;
		return -1;
	}
	VK_ASSERT(vkResetFences(Gfx::Device::GetInstance()->GetDevice(), 1, &Frames[FrameIndex].RenderFence), "vkResetFences failed !");
	VK_ASSERT(vkResetCommandBuffer(Frames[FrameIndex].MainCommandBuffer, 0), "vkResetCommandBuffer failed!");

	return swapchainimageindex;
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
	MainRT->SetFormat(VK_FORMAT_B8G8R8A8_SRGB);
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

void Gfx::Renderer::CopyToSwapchain(VkCommandBuffer cmd, RenderTarget* rt, uint32_t swapchainimageindex)
{
	{
		VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = rt->GetImage();
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        // VkPipelineStageFlags sourceStage;
        // VkPipelineStageFlags destinationStage;
  		// barrier.srcAccessMask = 0;
        //barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            cmd,
             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
	}


	VkOffset3D blitSize;
	blitSize.x = Gfx::Device::GetInstance()->GetSwapchainExtent().width;
	blitSize.y = Gfx::Device::GetInstance()->GetSwapchainExtent().height;
	blitSize.z = 1;
	std::cout << "swapchain size: " << blitSize.x << "-----" << blitSize.y << std::endl;
	VkImageBlit imageBlitRegion{};
	imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.srcSubresource.layerCount = 1;
	imageBlitRegion.srcOffsets[1] = blitSize;
	imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.dstSubresource.layerCount = 1;
	imageBlitRegion.dstOffsets[1] = blitSize;

	vkCmdBlitImage(
		cmd,
		rt->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		Gfx::Device::GetInstance()->GetSwapchainImage(swapchainimageindex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageBlitRegion,
		VK_FILTER_NEAREST);
	//});

   VkImageMemoryBarrier memBarrier = {};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.pNext = NULL;
    memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    memBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    memBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memBarrier.subresourceRange.baseMipLevel = 0;
    memBarrier.subresourceRange.levelCount = 1;
    memBarrier.subresourceRange.baseArrayLayer = 0;
    memBarrier.subresourceRange.layerCount = 1;
    memBarrier.image = Gfx::Device::GetInstance()->GetSwapchainImage(swapchainimageindex);
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
                         &memBarrier);

	// vkCmdCopyImage(info.cmd, bltSrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, bltDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //                1, &cregion);

    VkImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier.pNext = NULL;
    prePresentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    prePresentBarrier.subresourceRange.baseMipLevel = 0;
    prePresentBarrier.subresourceRange.levelCount = 1;
    prePresentBarrier.subresourceRange.baseArrayLayer = 0;
    prePresentBarrier.subresourceRange.layerCount = 1;
    prePresentBarrier.image = Gfx::Device::GetInstance()->GetSwapchainImage(swapchainimageindex);
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1,
                         &prePresentBarrier);

	
}

void Gfx::Renderer::CreateCommands()
{
	vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) vkGetInstanceProcAddr(Gfx::Vulkan::GetInstance()->GetVkInstance(), "vkCmdBeginRenderingKHR");
	vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) vkGetInstanceProcAddr(Gfx::Vulkan::GetInstance()->GetVkInstance(), "vkCmdEndRenderingKHR");

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
