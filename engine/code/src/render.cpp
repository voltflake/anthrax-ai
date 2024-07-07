#include "anthraxAI/vkengine.h"

void Engine::renderscene(VkCommandBuffer cmd, RenderObject* first, int rqsize) {

	preparecamerabuffer();
		//animator.Update(deltatime.count());

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;

//  ----------------------------------------------------------------------------------------
// 	SUBPASS 0: WRITE TO SHADER
//  ----------------------------------------------------------------------------------------
	for (int i = 0; i < rqsize; i++)
	{
		RenderObject& object = first[i];
		
		if (object.type == TYPE_GIZMO || object.type == TYPE_MODEL) {
			for (int i = 0; i < object.model->meshes.size(); i++) {
			if (object.type == TYPE_GIZMO && !gizmomove.visible) continue;

			if (object.animated) {
				updatebones(object.ID);
			}

			if (object.material != lastMaterial) {
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinewrite);
				lastMaterial = object.material;
				uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * (FrameIndex % MAX_FRAMES_IN_FLIGHT);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameIndex], 1, &uniformoffset);
			
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 2, 1, &Builder.getstorageset()[FrameIndex], 0, nullptr);
			}
			

			MeshPushConstants constants;
			constants.debugbones = debugbones;			
			constants.boneind = debugboneID;
			constants.objectID = object.ID;
			constants.debug = object.selected && gizmomove.visible ? 1 : 0;

			glm::mat4 model = glm::mat4(1.0f);
            model = ((glm::translate(model, glm::vec3(object.pos.x, object.pos.y, object.pos.z))));

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
		else {
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
	}
//  ----------------------------------------------------------------------------------------
// 	SUBPASS 1: WRITE TO SHADER
//  ----------------------------------------------------------------------------------------
	vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Builder.getreadpipeline());
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Builder.getreadpipelayout(), 1, 1, &Builder.getattachmentset(), 0, nullptr);
	vkCmdDraw(cmd, 6, 1, 0, 0);

//  ----------------------------------------------------------------------------------------
//  OBJECT SELECTION
//  ----------------------------------------------------------------------------------------
	mousepicking();

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

	VkCommandBuffer cmd = Builder.getframes()[FrameIndex].MainCommandBuffer;

	VkCommandBufferBeginInfo cmdbegininfo = {};
	cmdbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbegininfo.pNext = nullptr;

	cmdbegininfo.pInheritanceInfo = nullptr;
	cmdbegininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdbegininfo), "failed to begin a command buffer!");

//--------------------------------------------------------------
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	VkClearValue depthClear;
	depthClear.depthStencil = {1.0f, 0};
	VkClearValue clearValues[] = { clearValue, clearValue, depthClear };

	VkRenderPassBeginInfo rpinfo = Builder.beginrenderpass(static_cast<ClearFlags>(CLEAR_COLOR | CLEAR_DEPTH), Builder.getrenderpass(), Builder.getswapchainextent(), Builder.getframebuffers()[swapchainimageindex]);
	rpinfo.clearValueCount = 3;
	rpinfo.pClearValues = &clearValues[0];
//--------------------------------------------------------------

	vkCmdBeginRenderPass(cmd, &rpinfo, VK_SUBPASS_CONTENTS_INLINE);

	renderscene(cmd, Builder.getrenderqueue().data(), Builder.getrenderqueue().size());

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

	resources[TYPE_BACKGROUND] = {"check/back.jpg", {0,0,0}};
	std::string checkstr = "check/" + checkimgs[checkimg];
	std::cout << checkstr << '\n';
	resources[TYPE_OBJECT] = {checkstr, {0,0,0}};

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
