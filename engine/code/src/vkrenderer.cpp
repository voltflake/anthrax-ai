
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

	VkAttachmentDescription colorAttachment{};
    colorAttachment.format = devicehandler->getswapchainformat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depth_attachment = {};
    // Depth attachment
    depth_attachment.flags = 0;
    depth_attachment.format = VK_FORMAT_D32_SFLOAT;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.pResolveAttachments = nullptr;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency depthdependency = {};
	depthdependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depthdependency.dstSubpass = 0;
	depthdependency.srcAccessMask = 0;
	depthdependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depthdependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depthdependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	VkAttachmentDescription attachments[2] = { colorAttachment, depth_attachment };
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	VkSubpassDependency dependencies[2] = { dependency, depthdependency };
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = &dependencies[0];


	VK_ASSERT(vkCreateRenderPass(devicehandler->getlogicaldevice(), &renderPassInfo, nullptr, &renderpass), "failder to create render pass !");

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

void RenderBuilder::builframebuffers() {

	VkFramebufferCreateInfo fbinfo = {};
	fbinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbinfo.pNext = nullptr;

	fbinfo.renderPass = renderpass;
	fbinfo.attachmentCount = 1;
	fbinfo.width = devicehandler->getswapchainextent().width;
	fbinfo.height = devicehandler->getswapchainextent().height;
	fbinfo.layers = 1;

	const uint32_t swapchainimagecount = devicehandler->getswapchainimage().size();
	framebuffers = std::vector<VkFramebuffer>(swapchainimagecount);
	std::vector<VkImageView> imageviews = devicehandler->getswapchainimageview();
	for (int i = 0; i < swapchainimagecount; i++) {
		VkImageView attachments[2];
		attachments[0] = imageviews[i];
		attachments[1] = devicehandler->depthimage.texture->imageview;

		fbinfo.pAttachments = attachments;
		fbinfo.attachmentCount = 2;

		VK_ASSERT(vkCreateFramebuffer(devicehandler->getlogicaldevice(), &fbinfo, nullptr, &framebuffers[i]), "faild to create frame buffer!");
	}
}

void RenderBuilder::recreateframebuffer() {
	
	clearframebuffers();
	builframebuffers();

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