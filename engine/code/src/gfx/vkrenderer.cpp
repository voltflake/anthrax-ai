#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/core/scene.h"
#include "anthraxAI/gfx/bufferhelper.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gameobjects/objects/camera.h"
#include "anthraxAI/utils/debug.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/types.h>

void Gfx::Renderer::DrawSimple(Gfx::RenderObject& object)
{
    bool bindpipe, bindindex = false;
	CheckTmpBindings(object.Mesh, object.Material, &bindpipe, &bindindex);
	
    if (bindpipe) {
        vkCmdBindDescriptorSets(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, object.Material->PipelineLayout, 0, 1, Gfx::DescriptorsBase::GetInstance()->GetBindlessSet(), 0, nullptr);

		vkCmdBindPipeline(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, object.Material->Pipeline);
    }

	Gfx::MeshPushConstants constants;
	constants.texturebind = object.TextureBind;
	constants.bufferbind = object.BufferBind;
    constants.selected = 0;
    constants.boneID = -1;
    if (object.HasStorage) {
        constants.storagebind = object.StorageBind;
        constants.instancebind = object.InstanceBind;
        constants.objectID = object.ID;
        constants.selected = (object.IsSelected || object.ID == Core::Scene::GetInstance()->GetSelectedID()) ? 1 : 0;
        constants.boneID = Utils::Debug::GetInstance()->BoneID;
    }
	vkCmdPushConstants(Cmd.GetCmd(), object.Material->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Gfx::MeshPushConstants), &constants);
	vkCmdDraw(Cmd.GetCmd(), 6, 1, 0, 0);
}

void Gfx::Renderer::DrawMeshes(Gfx::RenderObject& object)
{
	const int meshsize = object.Model->Meshes.size();
	for (int i = 0; i < meshsize; i++) {

		DrawMesh(object, object.Model->Meshes[i], true);
	} 
}

void Gfx::Renderer::DrawMesh(Gfx::RenderObject& object, Gfx::MeshInfo* mesh, bool ismodel)
{
	bool bindpipe, bindindex = false;
	CheckTmpBindings(mesh, object.Material, &bindpipe, &bindindex);

	if (bindpipe) {
	    vkCmdBindDescriptorSets(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, object.Material->PipelineLayout, 0, 1, Gfx::DescriptorsBase::GetInstance()->GetBindlessSet(), 0, nullptr);
		vkCmdBindPipeline(Cmd.GetCmd(), VK_PIPELINE_BIND_POINT_GRAPHICS, object.Material->Pipeline);
    }

	Gfx::MeshPushConstants constants;
	constants.texturebind = object.TextureBind;
	constants.bufferbind = object.BufferBind;
    constants.selected = 0;
    constants.boneID = -1;
    if (object.HasStorage) {
        constants.storagebind = object.StorageBind;
        constants.instancebind = object.InstanceBind;
        constants.objectID = object.ID;
        constants.selected = (object.IsSelected || object.ID == Core::Scene::GetInstance()->GetSelectedID()) ? 1 : 0;
        if (Utils::Debug::GetInstance()->Bones) {
            constants.boneID = Utils::Debug::GetInstance()->BoneID;
        }
        constants.gizmo = object.GizmoType;
    }
	vkCmdPushConstants(Cmd.GetCmd(), object.Material->PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Gfx::MeshPushConstants), &constants);

	if (bindindex) {
		VkDeviceSize offset = {0};
		vkCmdBindVertexBuffers(Cmd.GetCmd(), 0, 1, &mesh->VertexBuffer.Buffer, &offset);
		vkCmdBindIndexBuffer(Cmd.GetCmd(), mesh->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT16);
	}
	if (ismodel) {
		vkCmdDrawIndexed(Cmd.GetCmd(), static_cast<uint32_t>(mesh->AIindices.size()), 1, 0, 0, InstanceIndex);
        InstanceIndex++;
	}
	else {
		vkCmdDrawIndexed(Cmd.GetCmd(), static_cast<uint32_t>(mesh->Indices.size()), 1, 0, 0, 0);		
	}
}

void Gfx::Renderer::Draw(Gfx::RenderObject& object)
{
    bool bindpipe, bindindex = false;
    if (object.Model) {
		DrawMeshes(object);
	}
	else {
		DrawMesh(object, object.Mesh, false);
	}
}

VkFormat Gfx::Renderer::GetRTFormat(Gfx::AttachmentFlags flag)
{
    if ((flag & Gfx::RENDER_ATTACHMENT_MASK) == Gfx::RENDER_ATTACHMENT_MASK) {
        return MaskRT->GetFormat();
    }
    return MainRT->GetFormat();
}

VkImageView Gfx::Renderer::GetRTImageView(AttachmentFlags flag)
{
	if (flag == Gfx::RENDER_ATTACHMENT_COLOR) {
        return MainRT->GetImageView();
    } 
    return MaskRT->GetImageView();
}

VkRenderingAttachmentInfoKHR* Gfx::Renderer::GetAttachmentInfo(AttachmentFlags flag, AttachmentRules loadop)
{
	VkClearValue clearvalue;
	if (flag == Gfx::RENDER_ATTACHMENT_COLOR || flag == Gfx::RENDER_ATTACHMENT_MASK) {
		clearvalue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

        AttachmentInfos[flag].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        AttachmentInfos[flag].imageView = GetRTImageView(flag);
        AttachmentInfos[flag].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        AttachmentInfos[flag].resolveMode = VK_RESOLVE_MODE_NONE;
        AttachmentInfos[flag].loadOp = ((loadop & Gfx::ATTACHMENT_RULE_LOAD) == Gfx::ATTACHMENT_RULE_LOAD) ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
        AttachmentInfos[flag].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        AttachmentInfos[flag].clearValue = clearvalue;

	}
	if (flag == Gfx::RENDER_ATTACHMENT_DEPTH) {
		clearvalue.depthStencil = {1.0f, 0};

		AttachmentInfos[flag].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		AttachmentInfos[flag].imageView = DepthRT->GetImageView();
		AttachmentInfos[flag].imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		AttachmentInfos[flag].resolveMode = VK_RESOLVE_MODE_NONE;
		AttachmentInfos[flag].loadOp = ((loadop & Gfx::ATTACHMENT_RULE_LOAD) == Gfx::ATTACHMENT_RULE_LOAD) ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
		AttachmentInfos[flag].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		AttachmentInfos[flag].clearValue = clearvalue;
	}
	return &AttachmentInfos[flag];
}

void Gfx::Renderer::CheckTmpBindings(Gfx::MeshInfo* mesh, Gfx::Material* material, bool* bindpipe, bool* bindindex)
{
	*bindpipe = TmpBindMaterial != material;
    *bindindex = TmpBindMesh != mesh;
    TmpBindMaterial = *bindpipe ? material : TmpBindMaterial;
	TmpBindMesh = *bindindex ? mesh : TmpBindMesh;
}

void Gfx::Renderer::NullTmpBindings()
{
    TmpBindMaterial = nullptr;
	TmpBindMesh = nullptr;
}

bool Gfx::Renderer::BeginFrame()
{
    Gfx::Renderer::GetInstance()->NullTmpBindings();
	ImGui::Render();

	SwapchainIndex = SyncFrame();
    if (SwapchainIndex == -1) {
        return false;
    }
    Cmd.SetCmd(GetFrame().MainCommandBuffer);
	Cmd.BeginCmd(Cmd.InfoCmd(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
    return true;
}

void Gfx::Renderer::EndRender()
{
    EndRendering(Cmd.GetCmd());
}
void Gfx::Renderer::StartRender(AttachmentFlags attachmentflags, AttachmentRules rules)
{
   	std::vector<RenderingAttachmentInfo> attachmentinfo;
	attachmentinfo.reserve(Gfx::RENDER_ATTACHMENT_SIZE);
	if ((attachmentflags & Gfx::RENDER_ATTACHMENT_COLOR) == Gfx::RENDER_ATTACHMENT_COLOR) {
		Gfx::RenderingAttachmentInfo info;
		info.IsDepth = false;
		info.Image = MainRT->GetImage();
        if ((rules & Gfx::ATTACHMENT_RULE_LOAD) == Gfx::ATTACHMENT_RULE_LOAD) {
            info.Layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
		info.Info = GetAttachmentInfo(Gfx::RENDER_ATTACHMENT_COLOR, rules);
		attachmentinfo.push_back(info);
	}
    if ((attachmentflags & Gfx::RENDER_ATTACHMENT_MASK) == Gfx::RENDER_ATTACHMENT_MASK) {
		Gfx::RenderingAttachmentInfo info;
		info.IsDepth = false;
		info.Image = MaskRT->GetImage();
		info.Info = GetAttachmentInfo(Gfx::RENDER_ATTACHMENT_MASK, rules);
		attachmentinfo.push_back(info);
	}
	if ((attachmentflags & Gfx::RENDER_ATTACHMENT_DEPTH) == Gfx::RENDER_ATTACHMENT_DEPTH) {
		Gfx::RenderingAttachmentInfo info;
		info.IsDepth = true;
		info.Image = DepthRT->GetImage();
		info.Info = GetAttachmentInfo(Gfx::RENDER_ATTACHMENT_DEPTH, rules);
		attachmentinfo.push_back(info);
	}
	
	const VkRenderingInfo renderinfo = Cmd.GetRenderingInfo(attachmentinfo, {(int)Gfx::Device::GetInstance()->GetSwapchainExtent().width, (int)Gfx::Device::GetInstance()->GetSwapchainExtent().height});// Core::WindowManager::GetInstance()->GetScreenResolution());    
	BeginRendering(Cmd.GetCmd(), &renderinfo);
}

void Gfx::Renderer::RenderUI()
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd.GetCmd());
}

void Gfx::Renderer::EndFrame()
{
	Cmd.CopyImage(	GetMainRT()->GetImage(),
					GetMainRT()->GetSize(),
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					Gfx::Device::GetInstance()->GetSwapchainImage(SwapchainIndex),
					{(int)Gfx::Device::GetInstance()->GetSwapchainExtent().width, (int)Gfx::Device::GetInstance()->GetSwapchainExtent().height},
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	Cmd.MemoryBarrier(Gfx::Device::GetInstance()->GetSwapchainImage(SwapchainIndex),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
					Cmd.GetSubresourceMainRange());

    Cmd.EndCmd();

    VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;
	VkPipelineStageFlags waitstage2 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitstage2;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &GetFrame().PresentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &GetFrame().RenderSemaphore;
	submit.commandBufferCount = 1;
	VkCommandBuffer cmd = Cmd.GetCmd();
	submit.pCommandBuffers = &cmd;
	VK_ASSERT(vkQueueSubmit(Gfx::Device::GetInstance()->GetQueue(Gfx::GRAPHICS_QUEUE), 1, &submit, GetFrame().RenderFence), "failed to submit queue!");

	VkResult presentresult = Cmd.Present(
		Gfx::Device::GetInstance()->GetQueue(Gfx::GRAPHICS_QUEUE),
		Cmd.PresentInfo(
			&Gfx::Device::GetInstance()->GetSwapchain(),
			&GetFrame().RenderSemaphore,
			&SwapchainIndex
		)
	);

	if (presentresult == VK_ERROR_OUT_OF_DATE_KHR) {
       OnResize = true; 
	}

   SetFrameInd();
}

Gfx::RenderTarget* Gfx::Renderer::GetTexture(const std::string& name)
{
	TexturesMap::iterator it = Textures.find(name);
	if (it == Textures.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}

void Gfx::Renderer::PrepareStorageBuffer()
{
    /*if (!Core::WindowManager::GetInstance()->IsMousePressed()) {*/
    /*    u_int dst[DEPTH_ARRAY_SCALE] = {0};*/
    /*    BufferHelper::MapMemory(Gfx::DescriptorsBase::GetInstance()->GetStorageUBO(), sizeof(u_int) * DEPTH_ARRAY_SCALE, 0, dst);*/
    /*    //SelectedID = 0;*/
    /*    return;*/
    /*}*/
    
    //for (int i = 0; i < MAX_INSTANCES; i++) {
   
    void* storage;
    VkDeviceSize storagesize = sizeof(uint32_t) * DEPTH_ARRAY_SCALE;
    vkMapMemory(Gfx::Device::GetInstance()->GetDevice(),Gfx::DescriptorsBase::GetInstance()->GetStorageBufferMemory(), 0, storagesize, 0, (void**)&storage);
    uint32_t* u = static_cast<uint32_t*>(storage);

    int selectedID = -1;
    // TODO: improve pressision
    //printf("--------------------\n");
	if (Core::WindowManager::GetInstance()->IsMouseSelected()) {
    for (int i = 0; i < DEPTH_ARRAY_SCALE; i++) {
        if (u[i] != 0) {
            selectedID = u[i];
			Core::Scene::GetInstance()->SetSelectedID(selectedID);
            // printf("[%d][%d] \n ",i, u[i]);
    		 uint32_t dst[DEPTH_ARRAY_SCALE] = {0};
			memcpy(storage, dst, DEPTH_ARRAY_SCALE * sizeof(uint32_t));
			vkUnmapMemory(Gfx::Device::GetInstance()->GetDevice(),Gfx::DescriptorsBase::GetInstance()->GetStorageBufferMemory());
				Core::WindowManager::GetInstance()->ReleaseMouseSelected();

            return;
        }
    }
	

    if (selectedID == -1) {
		Core::Scene::GetInstance()->SetSelectedID(0);
   // printf("-----|%d|\n\n", selectedID);

    }

	}
    

   // printf("-----|%d|---\n", Core::Scene::GetInstance()->GetSelectedID());
	//}
		 uint32_t dst[DEPTH_ARRAY_SCALE] = {0};
    memcpy(storage, dst, DEPTH_ARRAY_SCALE * sizeof(uint32_t));
    vkUnmapMemory(Gfx::Device::GetInstance()->GetDevice(),Gfx::DescriptorsBase::GetInstance()->GetStorageBufferMemory());

    //printf("\n\n");
   
    
}

/*void Gfx::Renderer::GetTransforms(InstanceData* datas, Gfx::RenderObject obj, int i)*/
/*{*/
/*    std::vector<glm::mat4> bonevec = {}; */
/*                bonevec = Core::Scene::GetInstance()->UpdateAnimation(obj);*/
/**/
/*                for(int k = 0; (k < obj.Model->Bones.FinTransform.size() ); k++) {*/
/*                    datas[i].bonesmatrices[k] = obj.Model->Bones.FinTransform[k];//vec[i]*/
/*                }*/
/**/
/*}*/

void Gfx::Renderer::PrepareInstanceBuffer()
{
    const size_t buffersize = sizeof(InstanceData) * MAX_INSTANCES ;
    void* instancedata;
    vkMapMemory(Gfx::Device::GetInstance()->GetDevice(),Gfx::DescriptorsBase::GetInstance()->GetInstanceBufferMemory(), 0, buffersize, 0, (void**)&instancedata);
    
    InstanceData* datas = (InstanceData*)instancedata;
    /*for (int i = 0; i < InstanceCount; i++) {*/
    /*    datas[i].rendermatrix = glm::mat4(1.0f); */
    /*}*/
    int i = 0;
    //for (auto& it : Core::Scene::GetInstance()->GetScenes()) {
    Modules::ScenesMap map =  Core::Scene::GetInstance()->GetScenes();
    bool hasanim = false;

    for (Gfx::RenderObject& obj : map[Core::Scene::GetInstance()->GetCurrentScene()].GetRenderQueue()) {
        if (!obj.Model || !obj.IsVisible) continue;
        hasanim = Core::Scene::GetInstance()->HasAnimation(obj.ID);
        for (int j = 0; j < obj.Model->Meshes.size(); j++ ) {
        
            if (hasanim) {
               // printf("IIIIII ======================================== %d\n", i);
                for (int k = 0; k < obj.Model->Bones.Info.size(); k++) {
                    datas[i].bonesmatrices[k] = obj.Model->Bones.Info[k].FinTransform;
                }
                    //vec[i]
                /*std::thread upd(&Gfx::Renderer::GetTransforms, this, datas, obj, i);*/
                /*upd.join();*/
                            }
            datas[i].hasanimation = hasanim ? 1 : 0;
            datas[i].rendermatrix =glm::translate(glm::mat4(1.0f), glm::vec3(obj.Position.x, obj.Position.y, obj.Position.z));// * CamData.view *  ;
            //glm::mat4(1.0f);
            i++;
        }
    }
    for (Gfx::RenderObject& obj : map["gizmo"].GetRenderQueue()) {
         if (!obj.Model || !obj.IsVisible) continue;
        for (int j = 0; j < obj.Model->Meshes.size(); j++ ) {
            float dist = glm::distance(glm::vec3(CamData.viewpos.x, CamData.viewpos.y, CamData.viewpos.z), glm::vec3(obj.Position.x, obj.Position.y, obj.Position.z) )* 0.05;
            if (dist <= 0.5) {
                dist = 0.5;
            }
            

            glm::vec3 distfin = glm::vec3(dist);


            datas[i].rendermatrix = glm::translate(glm::mat4(1.0f), glm::vec3(obj.Position.x, obj.Position.y, obj.Position.z));
            datas[i].rendermatrix = glm::scale(datas[i].rendermatrix, distfin); 
            i++;
        }

    }
    /*for (Gfx::RenderObject& obj : map["mask"].RenderQueue) {*/
    /*    if (!obj.Model || !obj.IsVisible) continue;*/
    /*    hasanim = Core::Scene::GetInstance()->HasAnimation(obj.ID);*/
    /*    for (int j = 0; j < obj.Model->Meshes.size(); j++ ) {*/
    /**/
    /*        if (hasanim) {*/
    /*            for (int k = 0; k < obj.Model->Bones.Info.size(); k++) {*/
    /*                datas[i].bonesmatrices[k] = obj.Model->Bones.Info[k].FinTransform;*/
    /*            }*/
    /*        }*/
    /*        datas[i].hasanimation = hasanim ? 1 : 0;*/
    /*        i++;*/
    /*    }*/
    /*}*/
    /**/

    InstanceCount = i;
    InstanceIndex = 0;
    //}
    vkUnmapMemory(Gfx::Device::GetInstance()->GetDevice(),Gfx::DescriptorsBase::GetInstance()->GetInstanceBufferMemory());

}
void Gfx::Renderer::PrepareCameraBuffer(Keeper::Camera& camera)
{
	glm::mat4 view = glm::lookAt(camera.GetPos(), camera.GetPos() + camera.GetFront(), camera.GetUp());
	glm::mat4 projection = glm::perspective(glm::radians(45.f), float(Gfx::Device::GetInstance()->GetSwapchainSize().x) / float(Gfx::Device::GetInstance()->GetSwapchainSize().y), 0.01f, 100.0f);
	projection[1][1] *= -1;

	CamData.model = glm::mat4(1.0f);
	CamData.proj = projection;
	CamData.view = view;
	CamData.viewproj = projection * view;
	CamData.viewpos = glm::vec4(camera.GetPos(), 1.0);
	CamData.mousepos = { Core::WindowManager::GetInstance()->GetMousePos().x, Core::WindowManager::GetInstance()->GetMousePos().y, 0, 0};
	CamData.viewport = { Gfx::Device::GetInstance()->GetSwapchainSize().x, Gfx::Device::GetInstance()->GetSwapchainSize().y , 0, 0 };//{ Core::WindowManager::GetInstance()->GetScreenResolution().x ,Core::WindowManager::GetInstance()->GetScreenResolution().y, 0, 0};
    CamData.time = static_cast<float>(Engine::GetInstance()->GetTimeSinceStart()) / 1000.0;

    const size_t buffersize = (sizeof(CameraData));
    BufferHelper::MapMemory(Gfx::DescriptorsBase::GetInstance()->GetCameraUBO(), buffersize, 0, &CamData);
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
    	OnResize = true;
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
	Core::Deletor::GetInstance()->Push(Core::Deletor::Type::SYNC, [=, this]() {
		vkDestroyFence(Gfx::Device::GetInstance()->GetDevice(), Upload.UploadFence, nullptr);
	});
	for (int i = 0; i < MAX_FRAMES; i++) {
		VK_ASSERT(vkCreateFence(Gfx::Device::GetInstance()->GetDevice(), &fencecreateinfo, nullptr, &Frames[i].RenderFence), "failder to create fence ! ");
	
		VK_ASSERT(vkCreateSemaphore(Gfx::Device::GetInstance()->GetDevice(), &semcreateinfo, nullptr, &Frames[i].PresentSemaphore), "failder to create present semaphore!");
		VK_ASSERT(vkCreateSemaphore(Gfx::Device::GetInstance()->GetDevice(), &semcreateinfo, nullptr, &Frames[i].RenderSemaphore), "failder to create render semaphore!");

		Core::Deletor::GetInstance()->Push(Core::Deletor::Type::SYNC, [=, this]() {
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
    if (MaskRT) {
        vkDestroySampler(Gfx::Device::GetInstance()->GetDevice(), *(MaskRT->GetSampler()), nullptr);
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetDeviceMemory(), nullptr);
		delete MaskRT;
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
	DepthRT->SetDimensions({(int)Gfx::Device::GetInstance()->GetSwapchainExtent().width, (int)Gfx::Device::GetInstance()->GetSwapchainExtent().height});//  Core::WindowManager::GetInstance()->GetScreenResolution());
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

    if (MaskRT) {
        vkDestroySampler(Gfx::Device::GetInstance()->GetDevice(), *(MaskRT->GetSampler()), nullptr);
		vkDestroyImageView(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetImageView(), nullptr);
		vkDestroyImage(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetImage(), nullptr);
		vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), MaskRT->GetDeviceMemory(), nullptr);
		delete MaskRT;
	}
	MaskRT = new RenderTarget(*MainRT);
    MaskRT->SetFormat(VK_FORMAT_R8_UNORM);
    MaskRT->CreateRenderTarget();
    CreateSampler(MaskRT);
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
  	vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) vkGetInstanceProcAddr(Gfx::Vulkan::GetInstance()->GetVkInstance(), "vkCmdBeginRenderingKHR");
	vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) vkGetInstanceProcAddr(Gfx::Vulkan::GetInstance()->GetVkInstance(), "vkCmdEndRenderingKHR");

    Gfx::QueueFamilyIndex indices = Gfx::Device::GetInstance()->FindQueueFimilies(Gfx::Device::GetInstance()->GetPhysicalDevice());
    VkCommandPoolCreateInfo poolinfo = CommandPoolCreateInfo(indices.Graphics.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    for (int i = 0; i < MAX_FRAMES; i++) {
       	VK_ASSERT(vkCreateCommandPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &Frames[i].CommandPool), "failed to create command pool!");
		
		VkCommandBufferAllocateInfo cmdinfo = CommandBufferCreateInfo(Frames[i].CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VK_ASSERT(vkAllocateCommandBuffers(Gfx::Device::GetInstance()->GetDevice(), &cmdinfo, &Frames[i].MainCommandBuffer), "failed to allocate command buffers!");

		Core::Deletor::GetInstance()->Push(Core::Deletor::Type::CMD, [=, this]() {
			vkDestroyCommandPool(Gfx::Device::GetInstance()->GetDevice(), Frames[i].CommandPool, nullptr);
		});
	}


	VK_ASSERT(vkCreateCommandPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &Upload.CommandPool), "failed to create upload command pool!");
	Core::Deletor::GetInstance()->Push(Core::Deletor::Type::CMD, [=, this]() {
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
