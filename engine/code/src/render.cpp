#include "anthraxAI/vkengine.h"
// create a CB class maybe? 
void Engine::render3d(VkCommandBuffer cmd, RenderObject& object, Mesh* lastMesh, Material* lastMaterial)
{
	int pointlightcounter = 0;
	for (int i = 0; i < object.model->meshes.size(); i++) {
		int k = 0;
		if ((object.type == TYPE_GIZMO && !gizmomove.visible) || (object.ID == TYPE_LIGHT && !camdata.hasdirectional)) continue;

		if (object.ID > TYPE_LIGHT + pointlightamount) {
			continue;
		}

		if (object.material != lastMaterial) {
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinewrite);
			lastMaterial = object.material;
			uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * (FrameIndex % MAX_FRAMES_IN_FLIGHT);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
		
			if (object.animated) {
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 2, 1, &Builder.getstorageset()[FrameIndex], 0, nullptr);

				updatebones(object.ID);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 3, 1, &Builder.getmodel(object.ID)->descritor, 0, nullptr);
			}
			else {
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 2, 1, &Builder.getstorageset()[FrameIndex], 0, nullptr);
			}
		}

		MeshPushConstants constants;
		constants.debugbones = Debug.bones;			
		constants.boneind = Debug.boneID;
		constants.objectID = object.ID;
		constants.debug = object.selected && gizmomove.visible ? 1 : 0;

		glm::mat4 model = glm::mat4(1.0f);
		model = ((glm::translate(model, glm::vec3(object.pos.x, object.pos.y, object.pos.z))));

		if (object.ID >= TYPE_LIGHT) {
			glm::vec3 scale = glm::vec3(0.2, 0.2, 0.2);
			model = glm::scale(model, scale);
		}
		constants.model = model;
		constants.rendermatrix = camdata.proj * camdata.view * model;
		vkCmdPushConstants(cmd, object.material->pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstants), &constants);
		
		if (object.model->meshes[i] != lastMesh) {
			VkDeviceSize offset = {0};
			vkCmdBindVertexBuffers(cmd, 0, 1, &object.model->meshes[i]->vertexbuffer.buffer, &offset);
			vkCmdBindIndexBuffer(cmd, object.model->meshes[i]->indexbuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			lastMesh = object.model->meshes[i];
		}
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 1, 1, &(*object.textureset), 0, nullptr);
		vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object.model->meshes[i]->aiindices.size()), 1, 0, 0, 0);
	}
}

void Engine::render2d(VkCommandBuffer cmd, RenderObject& object, Mesh* lastMesh, Material* lastMaterial)
{
	if (object.material != lastMaterial) {
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinewrite);
		lastMaterial = object.material;
		
		uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * (FrameIndex % MAX_FRAMES_IN_FLIGHT);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
	}
	MeshPushConstants constants;
	vkCmdPushConstants(cmd, object.material->pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstants), &constants);
	
	if (object.mesh != lastMesh && !object.debug) {
		VkDeviceSize offset = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->vertexbuffer.buffer, &offset);
		vkCmdBindIndexBuffer(cmd, object.mesh->indexbuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		lastMesh = object.mesh;
	}
	if (!object.debug) {
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 1, 1, &(*object.textureset), 0, nullptr);
		vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object.mesh->indices.size()), 1, 0, 0, 0);	
	}
	else {
		vkCmdDraw(cmd, 6, 1, 0, 0);
	}
}

void Engine::renderscene(VkCommandBuffer cmd) {

	preparecamerabuffer();

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;

//  ----------------------------------------------------------------------------------------
// 	SUBPASS 0: WRITE TO SHADER
//  ----------------------------------------------------------------------------------------
	std::vector<RenderObject> rq = Builder.getrenderqueue();
	
	for (RenderObject& object : rq) {
		//std::cout << "object type: " << object.type << '\n';
		if (object.type >= TYPE_MODEL) {
			render3d(cmd, object, lastMesh, lastMaterial);
		}
		else {
			render2d(cmd, object, lastMesh, lastMaterial);
		}
	}
	//std::cout << "-------------------\n\n";
//  ----------------------------------------------------------------------------------------
// 	SUBPASS 1: WRITE TO SHADER
//  ----------------------------------------------------------------------------------------
	// vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
	
	// vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Builder.getreadpipeline());
	// vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Builder.getreadpipelayout(), 1, 1, &Builder.getattachmentset(), 0, nullptr);
	// vkCmdDraw(cmd, 6, 1, 0, 0);

//  ----------------------------------------------------------------------------------------
//  OBJECT SELECTION
//  ----------------------------------------------------------------------------------------
	mousepicking();

}

void Engine::copytarget(VkCommandBuffer cmd, AllocatedImage& target, uint32_t swapchainindex)
{

}

void Engine::render() {

	ImGui::Render();

	if (Level.initres) {
		Level.initres = false;
		reloadresources();
	}

	VK_ASSERT(vkWaitForFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence, true, 1000000000), "vkWaitForFences failed !");
	uint32_t swapchainimageindex;
	VkResult e = vkAcquireNextImageKHR(Builder.getdevice(), Builder.getswapchain(), 1000000000, Builder.getframes()[FrameIndex].PresentSemaphore, VK_NULL_HANDLE, &swapchainimageindex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        winprepared = true;
		return ;
	}
	VK_ASSERT(vkResetFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence), "vkResetFences failed !");
	VK_ASSERT(vkResetCommandBuffer(Builder.getframes()[FrameIndex].MainCommandBuffer, 0), "vkResetCommandBuffer failed!");

	Cmd.set(Builder.getframes()[FrameIndex].MainCommandBuffer);
	Cmd.begin(Cmd.cmdinfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));

// const VkImageMemoryBarrier image_memory_barrier1 {
//     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//     .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//     .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//     .newLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//     .image = Builder.getswapchainimage()[swapchainimageindex],
//     .subresourceRange = {
//       .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//       .baseMipLevel = 0,
//       .levelCount = 1,
//       .baseArrayLayer = 0,
//       .layerCount = 1,
//     }
// };

// vkCmdPipelineBarrier(
//     Cmd.get(),
//     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
//     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
//     0,
//     0,
//     nullptr,
//     0,
//     nullptr,
//     1, // imageMemoryBarrierCount
//     &image_memory_barrier1 // pImageMemoryBarriers
// );

const VkImageMemoryBarrier image_memory_barrier3 {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    .image = Builder.devicehandler.mainrendertarget.texture->image,
    .subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    }
};

vkCmdPipelineBarrier(
    Cmd.get(),
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
    0,
    0,
    nullptr,
    0,
    nullptr,
    1, // imageMemoryBarrierCount
    &image_memory_barrier3 // pImageMemoryBarriers
);

VkImageSubresourceRange range{};
		range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel   = 0;
		range.levelCount     = 1;
		range.baseArrayLayer = 0;
		range.layerCount     = 1;

		VkImageSubresourceRange depth_range{range};
		depth_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

const VkImageMemoryBarrier image_memory_barrier2 {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    .image = Builder.devicehandler.depthimage.texture->image,
    .subresourceRange = depth_range,
};

vkCmdPipelineBarrier(
    Cmd.get(),
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
    0,
    0,
    nullptr,
    0,
    nullptr,
    1, // imageMemoryBarrierCount
    &image_memory_barrier2 // pImageMemoryBarriers
);

	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	VkClearValue depthClear;
	depthClear.depthStencil = {1.0f, 0};
	VkClearValue clearValues[] = { clearValue, clearValue, depthClear };


	// const VkRenderingAttachmentInfoKHR color_attachment_info {
    // .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
    // .imageView = Builder.devicehandler.mainrendertarget.texture->imageview,
    // .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
    // .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    // .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    // .clearValue = clearValues[0],
	// };

	const VkRenderingAttachmentInfoKHR main_color_attachment_info {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
    .imageView = Builder.devicehandler.mainrendertarget.texture->imageview,
    .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .clearValue = clearValues[0],
	};


	VkRenderingAttachmentInfoKHR depth_attachment_info {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
    .imageView = Builder.devicehandler.depthimage.texture->imageview,
    .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	.resolveMode = VK_RESOLVE_MODE_NONE,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .clearValue = clearValues[2],
	};
	
	// VkRenderingAttachmentInfoKHR col_atts[2] = {color_attachment_info, main_color_attachment_info};
	VkRect2D renderarea = VkRect2D{VkOffset2D{}, Builder.getswapchainextent()};
	const VkRenderingInfoKHR render_info {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.renderArea = renderarea,
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &main_color_attachment_info,
		.pDepthAttachment = &depth_attachment_info,
	};

vkCmdBeginRenderingKHR(Cmd.get(), &render_info);
//--------------------------------------------------------------
// 	VkClearValue clearValue;
// 	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
// 	VkClearValue depthClear;
// 	depthClear.depthStencil = {1.0f, 0};
// 	VkClearValue clearValues[] = { clearValue, clearValue, depthClear };

// 	VkRenderPassBeginInfo rpinfo = Cmd.rpinfo(static_cast<ClearFlags>(CLEAR_COLOR | CLEAR_DEPTH), Builder.getrenderpass(), Builder.getswapchainextent(), Builder.getframebuffers()[swapchainimageindex]);
// 	rpinfo.clearValueCount = 3;
// 	rpinfo.pClearValues = &clearValues[0];
// //--------------------------------------------------------------
// 	Cmd.beginrp(
// 		rpinfo,
// 	 	VK_SUBPASS_CONTENTS_INLINE);

	renderscene(Cmd.get());

 	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd.get());

	vkCmdEndRenderingKHR(Cmd.get());

	Builder.copycheck(Cmd.get(), Builder.devicehandler.mainrendertarget, swapchainimageindex);

	//copytarget(Cmd.get(), Builder.devicehandler.mainrendertarget, swapchainimageindex);
// const VkImageMemoryBarrier image_memory_barrier {
//     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//     .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
//     .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//     .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//     .image = Builder.getswapchainimage()[swapchainimageindex],
//     .subresourceRange = {
//       .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//       .baseMipLevel = 0,
//       .levelCount = 1,
//       .baseArrayLayer = 0,
//       .layerCount = 1,
//     }
// };

// vkCmdPipelineBarrier(
//     Cmd.get(),
//     VK_PIPELINE_STAGE_TRANSFER_BIT,  // srcStageMask
//     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
//     0,
//     0,
//     nullptr,
//     0,
//     nullptr,
//     1, // imageMemoryBarrierCount
//     &image_memory_barrier // pImageMemoryBarriers
// );

// 	Cmd.endrp();
	Cmd.end();

// this produce werid errors in validation layer

	// Cmd.submit(
	// 	Builder.getqueue().graphicsqueue,
	// 	Cmd.submitinfo(
	// 		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
	// 		&Builder.getframes()[FrameIndex].PresentSemaphore,
	// 		&Builder.getframes()[FrameIndex].RenderSemaphore),
	// 	&Builder.getframes()[FrameIndex].RenderFence
	// );

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;
	VkPipelineStageFlags waitstage2 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitstage2;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &Builder.getframes()[FrameIndex].PresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &Builder.getframes()[FrameIndex].RenderSemaphore;
	submit.commandBufferCount = 1;
	VkCommandBuffer cmd = Cmd.get();
	submit.pCommandBuffers = &cmd;
	VK_ASSERT(vkQueueSubmit(Builder.getqueue().graphicsqueue, 1, &submit, Builder.getframes()[FrameIndex].RenderFence), "failed to submit queue!");

	VkResult presentresult = Cmd.present(
		Builder.getqueue().graphicsqueue,
		Cmd.presentinfo(
			&Builder.getswapchain(),
			&Builder.getframes()[FrameIndex].RenderSemaphore,
			&swapchainimageindex
		)
	);

	if (presentresult == VK_ERROR_OUT_OF_DATE_KHR) {
		winprepared = true;
	}
	FrameIndex = (FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

