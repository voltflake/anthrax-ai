#include "../includes/vkengine.h"

void Engine::drawobjects(VkCommandBuffer cmd, RenderObject* first, int rqsize) {
	glm::vec3 camPos =  { 0.f,0.f,-20.f + zoomtest};
	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	static float f = 0;
	f += 0.2f;
	glm::mat4 modelm = glm::rotate(glm::mat4{ 1.0f },  glm::radians(f + 45.0f), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(60.f), static_cast<float>(Builder.getswapchainextent().width / Builder.getswapchainextent().height), 0.1f, 200.0f);
	projection[1][1] *= -1;

	// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::mat4 modell = glm::mat4(1.0f);
	modell = glm::translate(modell, glm::vec3(camdata.lightpos.x,camdata.lightpos.y,camdata.lightpos.z));
	modell = glm::scale(modell, glm::vec3(0.2f));

	camdata.model = modell;
	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;
	camdata.viewpos = glm::vec4(camPos.x, camPos.y, camPos.z, 1.0);
	camdata.pos = {mousepos.x, mousepos.y, 0, 0};
	camdata.viewport = {WindowExtend.width, WindowExtend.height, 0, 0};

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

		if (object.model) {
			if (object.material != lastMaterial) {

				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float) Builder.getswapchainextent().width;
				viewport.height = (float) Builder.getswapchainextent().height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				vkCmdSetViewport(cmd, 0, 1, &viewport);
				VkRect2D scissor{};
				scissor.offset = {0, 0};
				scissor.extent = Builder.getswapchainextent();
				vkCmdSetScissor(cmd, 0, 1, &scissor);

				lastMaterial = object.material;
				
				uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData))  * frameIndex;
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
			}
			camPos =  { 0.f + object.pos.x ,0.f + object.pos.y ,-20.f + zoomtest};
			view = glm::translate(glm::mat4(1.f), camPos);
			glm::mat4 model = object.transformmatrix;
			MeshPushConstants constants;
			constants.render_matrix = projection * view * modelm;
			constants.debugcollision = static_cast<int>(object.debugcollision);

			vkCmdPushConstants(cmd, object.material->pipelinelayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
			
			if (object.mesh != lastMesh) {
				VkDeviceSize offset = {0};
				vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->vertexbuffer.buffer, &offset);
				lastMesh = object.mesh;
			}
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 1, 1, &(*object.textureset), 0, nullptr);
		
			vkCmdDraw(cmd, object.mesh->vertices.size(), 1, 0, 0);
		}
		else {
			if (object.material != lastMaterial) {

				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
				lastMaterial = object.material;
				
				uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData))  * frameIndex;
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
			}
			glm::mat4 model = object.transformmatrix;
			MeshPushConstants constants;
			constants.render_matrix = projection * view * glm::mat4{ 1.0f } ;
			constants.debugcollision = static_cast<int>(object.debugcollision);

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
}

void Engine::draw() {

	ImGui::Render();

	if (Level.initres) {
		Level.initres = false;
		reloadresources();
	}

	VK_ASSERT(vkWaitForFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence, true, 1000000000), "vkWaitForFences failed !");
	
	uint32_t swapchainimageindex;
	VkResult e = vkAcquireNextImageKHR(Builder.getdevice(), Builder.getswapchain(), 1000000000, Builder.getframes()[FrameIndex].PresentSemaphore, nullptr, &swapchainimageindex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        winprepared = true;
		return ;
	}

	VK_ASSERT(vkResetFences(Builder.getdevice(), 1, &Builder.getframes()[FrameIndex].RenderFence), "vkResetFences failed !");

	VK_ASSERT(vkResetCommandBuffer(Builder.getframes()[FrameIndex].MainCommandBuffer, 0), "vkResetCommandBuffer failed!");


	VkCommandBuffer cmd = Builder.getframes()[FrameIndex].MainCommandBuffer;

	VkCommandBufferBeginInfo cmdbegininfo = {};
	cmdbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbegininfo.pNext = nullptr;

	cmdbegininfo.pInheritanceInfo = nullptr;
	cmdbegininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdbegininfo), "failed to begin a command buffer!");

	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	VkClearValue depthClear;
	depthClear.depthStencil = {1.0f, 0};
	VkClearValue clearValues[] = { clearValue, depthClear };
	VkRenderPassBeginInfo rpinfo = Builder.beginrenderpass(static_cast<ClearFlags>(CLEAR_COLOR | CLEAR_DEPTH), Builder.getrenderpass(), Builder.getswapchainextent(), Builder.getframebuffers()[swapchainimageindex]);
	rpinfo.clearValueCount = 2;
	rpinfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpinfo, VK_SUBPASS_CONTENTS_INLINE);

	// VkViewport viewport;
	// viewport.x = 0.0f;
	// viewport.y = 0.0f;
	// viewport.width = (float)Builder.devicehandler.getswapchainextent().width;
	// viewport.height = (float)Builder.devicehandler.getswapchainextent().height;
	// viewport.minDepth = 0.0f;
	// viewport.maxDepth = 1.0f;
	// vkCmdSetViewport(cmd, 0, 1, &viewport);
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
	if (Level.check2 && !Level.check) {
		countt++;
		if (countt > 5){
			Builder.copycheck(swapchainimageindex);
		}
	}
}

void Engine::loadmylevel() {

	playerpos.x = 0;
	playerpos.y = 0;
	Level.check = true;

	Builder.cleartextureset();
	Builder.clearimages();
	Builder.clearmeshes();

	resources.clear();

	Builder.renderqueue.clear();

	resources[TYPE_BACKGROUND] = {"check/back.jpg", {0,0}};
	std::string checkstr = "check/" + checkimgs[checkimg];
	std::cout << checkstr << '\n';
	resources[TYPE_OBJECT] = {checkstr, {0,0}};

	Builder.inittexture(resources);
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.buildpipeline(Level.check);

	Builder.initmeshbuilder();
	Builder.loadmeshes();

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

	Level.check = false;
	Level.check2 = true;

}
