
#include "../includes/vkengine.h"

void Engine::init() {
	initvulkan();

	initscene();

	initimgui();
}

void Engine::initvulkan() {

	Builder.buildinstance();

#ifdef OS_WINDOWS
	Builder.buildwinsurface(hwnd, hinstance);
	Builder.initdevicebuilder(hwnd);
#endif
#ifdef OS_LINUX
	Builder.buildlinuxsurface(connection, window);
	Builder.initdevicebuilder();
#endif
	Builder.buildphysicaldevice();

	Builder.buildlogicaldevice();

	Builder.buildswapchain();
	Builder.buildimagesview();
	
	Builder.initrenderbuilder();
	Builder.buildcommandpool();

	Builder.buildrenderpass();
	Builder.builframebuffers();
	
	Builder.startsync();

	Builder.inittexture();
	Builder.loadimages();

	Builder.initdescriptors();
	Builder.builddescriptors();

	Builder.initpipelinebuilder();
	Builder.buildpipeline();

	Builder.initmeshbuilder();
	Builder.loadmeshes();

}


void Engine::cleanup() {

	vkWaitForFences(Builder.getdevice(), 1, &Builder.getframes()[FrameNumber].RenderFence, true, 1000000000);

	Builder.cleanall();
}

void Engine::initscene() {

	// for (int x = -20; x <= 20; x++) {
	// 	for (int y = -20; y <= 20; y++) {

			RenderObject tri;
			tri.mesh = Builder.getmesh("triangle");
			tri.material = Builder.getmaterial("defaultmesh");
			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(10, 0, 10));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			tri.transformmatrix = translation * scale;

			Builder.pushrenderobject(tri);
	// 	}
	// }
	//Builder.descriptors.updatesamplerdescriptors();

}

void Engine::ui() {
       
    ImGui::ShowDemoWindow();
}


void Engine::drawobjects(VkCommandBuffer cmd, RenderObject* first, int count) {
	glm::vec3 camPos = { 0.f,-6.f,-10.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	CameraData camdata;
	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;

	float framed = (BgNumber/ 60.f);

	camdata.color = { sin(framed),0,cos(framed),1 };
	char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(CameraData));

  	vkMapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameNumber].devicememory, 0, sceneParamBufferSize, 0, (void**)&datadst);
   	
   	int frameIndex = FrameNumber % MAX_FRAMES_IN_FLIGHT;

	datadst += Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * frameIndex;

    memcpy( datadst, &camdata, (size_t)sizeof(CameraData));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameNumber].devicememory);

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	for (int i = 0; i < count; i++)
	{
		RenderObject& object = first[i];

		if (object.material != lastMaterial) {

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
			lastMaterial = object.material;
			
			uint32_t uniformoffset = Builder.descriptors.paduniformbuffersize(sizeof(CameraData))  * frameIndex;
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 0, 1, &Builder.getdescriptorset()[FrameNumber], 1, &uniformoffset);
		
			if (Builder.getsamplerset() != VK_NULL_HANDLE) {
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelinelayout, 1, 1, &Builder.getsamplerset(), 0, nullptr);
			}
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
		vkCmdDrawIndexed(cmd, static_cast<uint32_t>(object.mesh->indices.size()), 1, 0, 0, 0);

		//vkCmdDraw(cmd, object.mesh->vertices.size(), 1, 0, 0);
	}
}

void Engine::draw() {

	ImGui::Render();

	BgNumber++;
	VK_ASSERT(vkWaitForFences(Builder.getdevice(), 1, &Builder.getframes()[FrameNumber].RenderFence, true, 1000000000), "vkWaitForFences failed !");
	VK_ASSERT(vkResetFences(Builder.getdevice(), 1, &Builder.getframes()[FrameNumber].RenderFence), "vkResetFences failed !");

	VK_ASSERT(vkResetCommandBuffer(Builder.getframes()[FrameNumber].MainCommandBuffer, 0), "vkResetCommandBuffer failed!");

	uint32_t swapchainimageindex;
	VK_ASSERT(vkAcquireNextImageKHR(Builder.getdevice(), Builder.getswapchain(), 1000000000, Builder.getframes()[FrameNumber].PresentSemaphore, nullptr, &swapchainimageindex), "vkAcquireNextImageKHR failed!");

	VkCommandBuffer cmd = Builder.getframes()[FrameNumber].MainCommandBuffer;

	VkCommandBufferBeginInfo cmdbegininfo = {};
	cmdbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdbegininfo.pNext = nullptr;

	cmdbegininfo.pInheritanceInfo = nullptr;
	cmdbegininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_ASSERT(vkBeginCommandBuffer(cmd, &cmdbegininfo), "failed to begin a command buffer!");

	VkClearValue clearvalue;
	float flash = abs(sin(BgNumber / 120.f));
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
	submit.pWaitSemaphores = &Builder.getframes()[FrameNumber].PresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &Builder.getframes()[FrameNumber].RenderSemaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	VK_ASSERT(vkQueueSubmit(Builder.getqueue().graphicsqueue, 1, &submit, Builder.getframes()[FrameNumber].RenderFence), "failed to submit queue!");

	VkPresentInfoKHR presentinfo = {};
	presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentinfo.pNext = nullptr;
	presentinfo.pSwapchains = &Builder.getswapchain();
	presentinfo.swapchainCount = 1;
	presentinfo.pWaitSemaphores = &Builder.getframes()[FrameNumber].RenderSemaphore;
	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pImageIndices = &swapchainimageindex;

	VK_ASSERT(vkQueuePresentKHR(Builder.getqueue().graphicsqueue, &presentinfo), "vkQueuePresentKHR failed!");

	FrameNumber = (FrameNumber + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::initimgui() {
	
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_ASSERT(vkCreateDescriptorPool(Builder.getdevice(), &pool_info, nullptr, &imguiPool), "failed to creat imgui descriptor set!");


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	ImGui::CreateContext();
    ImGui_ImplX11_Init(connection, &window);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

ImGui::StyleColorsDark();
    io.Fonts->AddFontDefault();
	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Builder.getinstance();
	init_info.PhysicalDevice = Builder.getphysicaldevice();
	init_info.Device = Builder.getdevice();
	init_info.Queue = Builder.getqueue().graphicsqueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, Builder.getrenderpass());

	Builder.renderer.immediatesubmit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	Builder.deletorhandler.pushfunction([=]() {
		vkDestroyDescriptorPool(Builder.getdevice(), imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		});
}