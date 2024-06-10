
#include "anthraxAI/vkrenderer.h"

VkCommandBufferBeginInfo RenderBuilder::commandbufferbegininfo(VkCommandBufferUsageFlags flags /*= 0*/)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;

    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

VkSubmitInfo RenderBuilder::submitinfo(VkCommandBuffer* cmd)
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

VkCommandPoolCreateInfo RenderBuilder::commandpoolcreateinfo(uint32_t graphicsfamily, VkCommandPoolCreateFlags flags) {
	
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;

	info.queueFamilyIndex = graphicsfamily;
	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo RenderBuilder::commandbufferallcoteinfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level) {
	
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

VkFenceCreateInfo RenderBuilder::fencecreateinfo(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = flags;
    return fenceCreateInfo;
}

VkSemaphoreCreateInfo RenderBuilder::semaphorecreateinfo(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo semCreateInfo = {};
    semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semCreateInfo.pNext = nullptr;
    semCreateInfo.flags = flags;
    return semCreateInfo;
}

void RenderBuilder::buildcommandpool() {

	QueueFamilyIndex queuefamilyindices = devicehandler->findqueuefamilies(devicehandler->getphysicaldevice());

    VkCommandPoolCreateInfo poolInfo = commandpoolcreateinfo(queuefamilyindices.graphicsfamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

    	VK_ASSERT(vkCreateCommandPool(devicehandler->getlogicaldevice(), &poolInfo, nullptr, &frames[i].CommandPool), "failed to create command pool!");
		
		VkCommandBufferAllocateInfo cmdAllocInfo = commandbufferallcoteinfo(frames[i].CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VK_ASSERT(vkAllocateCommandBuffers(devicehandler->getlogicaldevice(), &cmdAllocInfo, &frames[i].MainCommandBuffer), "failed to allocate command buffers!");

		deletorhandler->pushfunction([=]() {
			vkDestroyCommandPool(devicehandler->getlogicaldevice(), frames[i].CommandPool, nullptr);
		});
	}

	VkCommandPoolCreateInfo uploadcommandpoolinfo = commandpoolcreateinfo(queuefamilyindices.graphicsfamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	VK_ASSERT(vkCreateCommandPool(devicehandler->getlogicaldevice(), &uploadcommandpoolinfo, nullptr, &uploadcontext.CommandPool), "failed to create upload command pool!");

	deletorhandler->pushfunction([=]() {
		vkDestroyCommandPool(devicehandler->getlogicaldevice(), uploadcontext.CommandPool, nullptr);
	});

	VkCommandBufferAllocateInfo cmdallocinfo = commandbufferallcoteinfo(uploadcontext.CommandPool, 1);

	VK_ASSERT(vkAllocateCommandBuffers(devicehandler->getlogicaldevice(), &cmdallocinfo, &uploadcontext.CommandBuffer), "failed to allocate upload command buffers!");
}


void RenderBuilder::submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VkCommandBuffer cmd = uploadcontext.CommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = commandbufferbegininfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdBeginInfo), "failed to begin command buffer!");

	function(cmd);

	VK_ASSERT(vkEndCommandBuffer(cmd), "failed to end command buffer!");

	VkSubmitInfo submit = submitinfo(&cmd);

	VK_ASSERT(vkQueueSubmit(devicehandler->getqueue().graphicsqueue, 1, &submit, uploadcontext.UploadFence), "failed to submit upload queue!");

	vkWaitForFences(devicehandler->getlogicaldevice(), 1, &uploadcontext.UploadFence, true, 9999999999);
	vkResetFences(devicehandler->getlogicaldevice(), 1, &uploadcontext.UploadFence);

	vkResetCommandPool(devicehandler->getlogicaldevice(), uploadcontext.CommandPool, 0);
}

void RenderBuilder::buildrenderpass() {

	std::array<VkAttachmentDescription, 3> attachments{};

	// Color attachment swapchain
	attachments[0].format = devicehandler->getswapchainformat();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// color attachment
	attachments[1].format = devicehandler->mainrendertarget.texture->format;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Depth attachment
	attachments[2].format = devicehandler->depthimage.texture->format;
	attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[2].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//  ----------------------------------------------------------------------------------------
// 	FIRST SUBPUSS: FILL COLOR INPUT IMAGE AND DEPTH BUFFER
//  ----------------------------------------------------------------------------------------

std::array<VkSubpassDescription,2> subpassdecriptions{};

	VkAttachmentReference colorinput = { 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentReference depthinput = { 2, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

	subpassdecriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassdecriptions[0].colorAttachmentCount = 1;
	subpassdecriptions[0].pColorAttachments = &colorinput;
	subpassdecriptions[0].pDepthStencilAttachment = &depthinput;
//  ----------------------------------------------------------------------------------------
// 	SECOND SUBPUSS: COPY FROM COLOR INPUT IMAGE TO SWAPCHAIN IMAGE
//  ----------------------------------------------------------------------------------------
	VkAttachmentReference colorReferenceSwapchain = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	subpassdecriptions[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassdecriptions[1].colorAttachmentCount = 1;
	subpassdecriptions[1].pColorAttachments = &colorReferenceSwapchain;
//  ----------------------------------------------------------------------------------------
// 	INUPT ATTACHMENT: TO BE USED AS REF ON THE SEC SUBPASS
//  ----------------------------------------------------------------------------------------
	VkAttachmentReference inputref[2];
	inputref[0] = { 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	inputref[1] = { 2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	subpassdecriptions[1].inputAttachmentCount = 2;
	subpassdecriptions[1].pInputAttachments = inputref;
//  ----------------------------------------------------------------------------------------
// 	SUBPASS DEPENDENCY: FOR IMAGE TRANSITION
//  ----------------------------------------------------------------------------------------
	std::array<VkSubpassDependency, 3> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//transition of input attachment from color_attachment to shader_read
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//transition from shader_read to memory_read
	dependencies[2].srcSubpass = 0;
	dependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[2].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	VkRenderPassCreateInfo rpinfo = {};
	rpinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	rpinfo.pAttachments = attachments.data();
	rpinfo.subpassCount = static_cast<uint32_t>(subpassdecriptions.size());
	rpinfo.pSubpasses = subpassdecriptions.data();
	rpinfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	rpinfo.pDependencies = dependencies.data();

	VK_ASSERT(vkCreateRenderPass(devicehandler->getlogicaldevice(), &rpinfo, nullptr, &renderpass), "failder to create render pass !");

	deletorhandler->pushfunction([=]() {
		vkDestroyRenderPass(devicehandler->getlogicaldevice(), renderpass, nullptr);
	});
}

void RenderBuilder::clearframebuffers() {
	const uint32_t swapchainimagecount = devicehandler->getswapchainimage().size();
	for (int i = 0; i < swapchainimagecount; i++) {
		vkDestroyFramebuffer(devicehandler->getlogicaldevice(), framebuffers[i], nullptr);
	}
}

void RenderBuilder::builframebuffers(DeviceBuilder& device) {

	VkFramebufferCreateInfo fbinfo = {};
	fbinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbinfo.pNext = nullptr;

	fbinfo.renderPass = renderpass;
	fbinfo.attachmentCount = 1;
	fbinfo.width = device.getswapchainextent().width;
	fbinfo.height = device.getswapchainextent().height;
	fbinfo.layers = 1;

	const uint32_t swapchainimagecount = device.getswapchainimage().size();
	framebuffers = std::vector<VkFramebuffer>(swapchainimagecount);
	std::vector<VkImageView> imageviews = device.getswapchainimageview();
	for (int i = 0; i < swapchainimagecount; i++) {
		VkImageView attachments[3];
		attachments[0] = imageviews[i];
		attachments[1] = device.mainrendertarget.texture->imageview;
		attachments[2] = device.depthimage.texture->imageview;

		fbinfo.pAttachments = attachments;
		fbinfo.attachmentCount = 3;

		VK_ASSERT(vkCreateFramebuffer(device.getlogicaldevice(), &fbinfo, nullptr, &framebuffers[i]), "faild to create frame buffer!");
	}
}

void RenderBuilder::recreateframebuffer(DeviceBuilder& device) {
	
	clearframebuffers();
	builframebuffers(device);

	std::cout << "frame buffer recreated\n";
}

void RenderBuilder::sync() {

	VkFenceCreateInfo fenceCreateInfo = fencecreateinfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkFenceCreateInfo uploadfencecreateinfo = fencecreateinfo();

	VkSemaphoreCreateInfo semaphoreCreateInfo = semaphorecreateinfo();

	VK_ASSERT(vkCreateFence(devicehandler->getlogicaldevice(), &uploadfencecreateinfo, nullptr, &uploadcontext.UploadFence), "failder to create upload fence ! ");

    deletorhandler->pushfunction([=]() {
		vkDestroyFence(devicehandler->getlogicaldevice(), uploadcontext.UploadFence, nullptr);
	});

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {     

		VK_ASSERT(vkCreateFence(devicehandler->getlogicaldevice(), &fenceCreateInfo, nullptr, &frames[i].RenderFence), "failder to create fence ! ");
        
        deletorhandler->pushfunction([=]() {
			vkDestroyFence(devicehandler->getlogicaldevice(), frames[i].RenderFence, nullptr);
		});

       	VK_ASSERT(vkCreateSemaphore(devicehandler->getlogicaldevice(), &semaphoreCreateInfo, nullptr, &frames[i].PresentSemaphore), "failder to create present semaphore!");
		VK_ASSERT(vkCreateSemaphore(devicehandler->getlogicaldevice(), &semaphoreCreateInfo, nullptr, &frames[i].RenderSemaphore), "failder to create render semaphore!");

		deletorhandler->pushfunction([=]() {
	        vkDestroySemaphore(devicehandler->getlogicaldevice(), frames[i].PresentSemaphore, nullptr);
	        vkDestroySemaphore(devicehandler->getlogicaldevice(), frames[i].RenderSemaphore, nullptr);
	    });
	}

}