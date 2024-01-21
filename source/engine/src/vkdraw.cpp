#include "../includes/vkengine.h"

void Engine::drawobjects(VkCommandBuffer cmd, RenderObject* first, int rqsize) {
	glm::vec3 camPos = { 0.f,-6.f,-10.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	CameraData camdata;
	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;
	camdata.pos = {mousepos.x, mousepos.y};

	char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(CameraData));

  	vkMapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory, 0, sceneParamBufferSize, 0, (void**)&datadst);
   	
   	int frameIndex = FrameIndex % MAX_FRAMES_IN_FLIGHT;

	datadst += Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * frameIndex;

    memcpy( datadst, &camdata, (size_t)sizeof(CameraData));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory);

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	for (int i = 0; i < rqsize; i++)
	{
		RenderObject& object = first[i];

		if (object.material != lastMaterial) {

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
			lastMaterial = object.material;
			
			uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData))  * frameIndex;
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
	
		}
		glm::mat4 model = object.transformmatrix;
		
		MeshPushConstants constants;
		constants.render_matrix = object.transformmatrix;

		vkCmdPushConstants(cmd, object.material->pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

		if (object.mesh != lastMesh) {
			VkDeviceSize offset = {0};
			vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->vertexbuffer.buffer, &offset);
            vkCmdBindIndexBuffer(cmd, object.mesh->indexbuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			lastMesh = object.mesh;
		}
	
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 1, 1, &(*object.textureset), 0, nullptr);

		vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object.mesh->indices.size()), 1, 0, 0, 0);
	}
}

void Engine::draw() {

	ImGui::Render();

	if (Levels.level.loaded) {
		Levels.level.loaded = false;
		reloadresources();
	}

	VK_ASSERT(vkWaitForFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence, true, 1000000000), "vkWaitForFences failed !");
	VK_ASSERT(vkResetFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence), "vkResetFences failed !");

	VK_ASSERT(vkResetCommandBuffer(Builder.getframes()[FrameIndex].MainCommandBuffer, 0), "vkResetCommandBuffer failed!");

	uint32_t swapchainimageindex;
	VkResult e = vkAcquireNextImageKHR(Builder.getdevice(), Builder.getswapchain(), 1000000000, Builder.getframes()[FrameIndex].PresentSemaphore, nullptr, &swapchainimageindex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        winprepared = true;       
		return ;
	}

	VkCommandBuffer cmd = Builder.getframes()[FrameIndex].MainCommandBuffer;

	VkCommandBufferBeginInfo cmdbegininfo = {};
	cmdbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbegininfo.pNext = nullptr;

	cmdbegininfo.pInheritanceInfo = nullptr;
	cmdbegininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdbegininfo), "failed to begin a command buffer!");

	VkClearValue clearvalue;
	clearvalue.color = { {0.0f, 0.0f, 0.0f, 0.0f } };

	VkRenderPassBeginInfo rpinfo = {};
	rpinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpinfo.pNext = nullptr;

	rpinfo.renderPass = Builder.getrenderpass();
	rpinfo.renderArea.offset.x = 0;
	rpinfo.renderArea.offset.y = 0;
	rpinfo.renderArea.extent = Builder.getswapchainextent();
	rpinfo.framebuffer = Builder.getframebuffers()[swapchainimageindex];
	rpinfo.clearValueCount = 1;
	rpinfo.pClearValues = &clearvalue;

	vkCmdBeginRenderPass(cmd, &rpinfo, VK_SUBPASS_CONTENTS_INLINE);

	drawobjects(cmd, Builder.getrenderqueue().data(), Builder.getrenderqueue().size());

 	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRenderPass(cmd);

	VK_ASSERT(vkEndCommandBuffer(cmd), "failder to end command buffer");

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitstage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitstage;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &Builder.getframes()[FrameIndex].PresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &Builder.getframes()[FrameIndex].RenderSemaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	VK_ASSERT(vkQueueSubmit(Builder.getqueue().graphicsqueue, 1, &submit, Builder.getframes()[FrameIndex].RenderFence), "failed to submit queue!");

	VkPresentInfoKHR presentinfo = {};
	presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentinfo.pNext = nullptr;
	presentinfo.pSwapchains = &Builder.getswapchain();
	presentinfo.swapchainCount = 1;
	presentinfo.pWaitSemaphores = &Builder.getframes()[FrameIndex].RenderSemaphore;
	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pImageIndices = &swapchainimageindex;

	VkResult presentresult = vkQueuePresentKHR(Builder.getqueue().graphicsqueue, &presentinfo);
	if (presentresult == VK_ERROR_OUT_OF_DATE_KHR) {
		winprepared = true;
	}

	FrameIndex = (FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	
	static int countt = 0;
	if (Levels.check2 && !Levels.check) {
		countt++;
		if (countt > 5){
			Builder.copycheck(swapchainimageindex);	
		}
	}
}

void Engine::loadmylevel() {

	playerpos.x = 0;
	playerpos.y = 0;
	Levels.check = true;

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearmeshes();

	resources.clear();

	Builder.renderqueue.clear();

	resources["check/back.jpg"] = {0,0};
	std::string checkstr = "check/" + checkimgs[checkimg];
	std::cout << checkstr << '\n';
	resources[checkstr] = {0,0};

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.buildpipeline(Levels.check);

	Builder.initmeshbuilder();
	Builder.loadmeshes(resources);

	int i = 0;
	Builder.descriptors.updatesamplerdescriptors2("check/back.jpg", checkstr);

	for (auto& list : resources) {

		RenderObject tri;
		tri.mesh = Builder.getmesh(list.first);
		tri.material = Builder.getmaterial("defaultmesh");

		tri.textureset = &Builder.getsamplerset()[i];
		Builder.pushrenderobject(tri);

		i++;
		break ;
	}

	Levels.check = false;
	Levels.check2 = true;

}
