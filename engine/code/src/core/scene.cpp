#include "anthraxAI/core/scene.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkrenderer.h"

void Core::Scene::RenderScene()
{
    uint32_t swapchainindex = Gfx::Renderer::GetInstance()->SyncFrame();

    Cmd.SetCmd(Gfx::Renderer::GetInstance()->GetFrame().MainCommandBuffer);
	Cmd.BeginCmd(Cmd.InfoCmd(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));


// const VkImageMemoryBarrier image_memory_barrier3 {
//     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//     .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//     .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//     .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//     .image = Gfx::Renderer::GetInstance()->GetMainRT()->GetImage(),
//     .subresourceRange = {
//       .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//       .baseMipLevel = 0,
//       .levelCount = 1,
//       .baseArrayLayer = 0,
//       .layerCount = 1,
//     }
// };

// vkCmdPipelineBarrier(
//     Cmd.GetCmd(),
//     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
//     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
//     0,
//     0,
//     nullptr,
//     0,
//     nullptr,
//     1, // imageMemoryBarrierCount
//     &image_memory_barrier3 // pImageMemoryBarriers
// );

// VkImageSubresourceRange range{};
// 		range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
// 		range.baseMipLevel   = 0;
// 		range.levelCount     = 1;
// 		range.baseArrayLayer = 0;
// 		range.layerCount     = 1;

// 		VkImageSubresourceRange depth_range{range};
// 		depth_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

// const VkImageMemoryBarrier image_memory_barrier2 {
//     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//     .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//     .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//     .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
//     .image = Gfx::Renderer::GetInstance()->GetDepthRT()->GetImage(),
//     .subresourceRange = depth_range,
// };

// vkCmdPipelineBarrier(
//     Cmd.GetCmd(),
//     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
//     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
//     0,
//     0,
//     nullptr,
//     0,
//     nullptr,
//     1, // imageMemoryBarrierCount
//     &image_memory_barrier2 // pImageMemoryBarriers
// );

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
    .imageView = Gfx::Renderer::GetInstance()->GetMainRT()->GetImageView(),
    .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
    .clearValue = clearValues[0],
	};


	VkRenderingAttachmentInfoKHR depth_attachment_info {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
    .imageView =  Gfx::Renderer::GetInstance()->GetDepthRT()->GetImageView(),
    .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	.resolveMode = VK_RESOLVE_MODE_NONE,
    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    .clearValue = clearValues[2],
	};
	
    Vector2<int> extents = Core::WindowManager::GetInstance()->GetScreenResolution();
	// VkRenderingAttachmentInfoKHR col_atts[2] = {color_attachment_info, main_color_attachment_info};
	VkRect2D renderarea = VkRect2D{VkOffset2D{}, { static_cast<uint32_t>(extents.x), static_cast<uint32_t>(extents.y) } };
	const VkRenderingInfoKHR render_info {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
		.renderArea = renderarea,
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &main_color_attachment_info,
		.pDepthAttachment = &depth_attachment_info,
	};
    
    //vkCmdBeginRenderingKHR(Cmd.GetCmd(), &render_info);
    Gfx::Renderer::GetInstance()->BeginRendering(Cmd.GetCmd(), &render_info);
    
    // //draw

    Gfx::Renderer::GetInstance()->PrepareCameraBuffer();



	
	vkCmdBindPipeline(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, RenderQueue[0].Material->Pipeline);

    VkDescriptorSet CameraSet;
    VkDescriptorBufferInfo caminfo;
    caminfo.buffer = Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(Gfx::Renderer::GetInstance()->GetFrameInd());
    caminfo.offset = 0;
    caminfo.range = sizeof(Gfx::CameraData);
    Gfx::Descriptors::Begin(Gfx::DescriptorsBase::GetInstance()->GetLayoutCache(), Gfx::DescriptorsBase::GetInstance()->GetAllocator())
        .BindBuffer(0, &caminfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build(CameraSet, Gfx::DescriptorsBase::GetInstance()->GetGlobalLayout());

    uint32_t uniformoffset = Gfx::DescriptorsBase::GetInstance()->PadUniformBufferSize(sizeof(Gfx::CameraData)) * (Gfx::Renderer::GetInstance()->GetFrameInd() % MAX_FRAMES);
	vkCmdBindDescriptorSets(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, RenderQueue[0].Material->PipelineLayout, 0, 1, &CameraSet, 1, &uniformoffset);

	Gfx::MeshPushConstants constants;
	vkCmdPushConstants(Cmd.GetCmd(), RenderQueue[0].Material->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Gfx::MeshPushConstants), &constants);

    VkDeviceSize offset = {0};
	vkCmdBindVertexBuffers(Cmd.GetCmd(), 0, 1, &RenderQueue[0].Mesh->VertexBuffer.Buffer, &offset);
	vkCmdBindIndexBuffer(Cmd.GetCmd(), RenderQueue[0].Mesh->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);

   VkDescriptorSet texturedesc;
       VkDescriptorImageInfo imageinfo = {};
    imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageinfo.sampler = *(Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSampler());
    imageinfo.imageView = Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetImageView();

    Gfx::Descriptors::Begin(Gfx::DescriptorsBase::GetInstance()->GetLayoutCache(), Gfx::DescriptorsBase::GetInstance()->GetAllocator())
        .BindImage(0, &imageinfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build(texturedesc, Gfx::DescriptorsBase::GetInstance()->GetTextureLayout());


	vkCmdBindDescriptorSets(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, RenderQueue[0].Material->PipelineLayout, 1, 1, &texturedesc, 0, nullptr);
	vkCmdDrawIndexed(Cmd.GetCmd(), static_cast<uint32_t>(RenderQueue[0].Mesh->Indices.size()), 1, 0, 0, 0);	


    Gfx::Renderer::GetInstance()->EndRendering(Cmd.GetCmd());
	// vkCmdEndRenderingKHR(Cmd.GetCmd());

    // copy image
    Gfx::Renderer::GetInstance()->CopyToSwapchain(Cmd.GetCmd(), Gfx::Renderer::GetInstance()->GetMainRT(), swapchainindex);

    Cmd.EndCmd();

    VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;
	VkPipelineStageFlags waitstage2 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitstage2;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &Gfx::Renderer::GetInstance()->GetFrame().PresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &Gfx::Renderer::GetInstance()->GetFrame().RenderSemaphore;
	submit.commandBufferCount = 1;
	VkCommandBuffer cmd = Cmd.GetCmd();
	submit.pCommandBuffers = &cmd;
	VK_ASSERT(vkQueueSubmit(Gfx::Device::GetInstance()->GetQueue(Gfx::GRAPHICS_QUEUE), 1, &submit, Gfx::Renderer::GetInstance()->GetFrame().RenderFence), "failed to submit queue!");

	VkResult presentresult = Cmd.Present(
		Gfx::Device::GetInstance()->GetQueue(Gfx::GRAPHICS_QUEUE),
		Cmd.PresentInfo(
			&Gfx::Device::GetInstance()->GetSwapchain(),
			&Gfx::Renderer::GetInstance()->GetFrame().RenderSemaphore,
			&swapchainindex
		)
	);

   // Gfx::Renderer::FinalizeRendering(Cmd);

	if (presentresult == VK_ERROR_OUT_OF_DATE_KHR) {
        // for resizzing

        //winprepared = true; 
	}

    Gfx::Renderer::GetInstance()->SetFrameInd();
}

void Core::Scene::LoadResources()
{
    Core::RenderObject test;

    test.Position = {0.0f};
    test.Material = Gfx::Pipeline::GetInstance()->GetMaterial("sprite");
    test.Mesh = Gfx::Mesh::GetInstance()->GetMesh();

    VkDescriptorImageInfo imageinfo = {};
    imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageinfo.sampler = *(Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetSampler());
    imageinfo.imageView = Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg").GetImageView();

    VkDescriptorSet texturedesc;
    Gfx::Descriptors::Begin(Gfx::DescriptorsBase::GetInstance()->GetLayoutCache(), Gfx::DescriptorsBase::GetInstance()->GetAllocator())
        .BindImage(0, &imageinfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .Build(texturedesc, Gfx::DescriptorsBase::GetInstance()->GetTextureLayout());

    test.Textureset = &texturedesc;

    RenderQueue.push_back(test);
    // test.textureset 
}