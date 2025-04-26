#pragma once

#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/bufferhelper.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkcmdhelper.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "glm/fwd.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Gfx
{
    class Renderer : public Utils::Singleton<Renderer>
    {
        public:
            void CleanTextures();

            void CreateCommands();
            void CreateRenderTargets();
            void DestroyRenderTarget(RenderTarget* rt);
            void CreateImGuiDescSet();

            void CleanResources();

            void CopyImage(Gfx::RenderTargetsList src_id, Gfx::RenderTargetsList dst_id);

            void CreateTextures();
            bool CreateTextureFromInfo(const std::string& texturename);
            RenderTarget CreateTexture(const std::string& path);
            void CreateSampler(RenderTarget& rt);
            void CreateSampler(RenderTarget* rt);

            TexturesMap GetTextureMap() const { return Textures; }
            RenderTarget* GetTexture(const std::string& path);

            RenderTarget* GetRT(Gfx::RenderTargetsList id) const { return RTs[id]; }
            std::vector<std::string> GetRTList();

            void PrepareCameraBuffer(Keeper::Camera& camera);
            void PrepareInstanceBuffer();
            void GetTransforms(InstanceData* datas, Gfx::RenderObject obj, int i);
            void PrepareStorageBuffer();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            void RenderUI();
            void TransferLayoutsDebug();

            int GetFrameInd() { return FrameIndex; }
            FrameData& GetFrame() { return Frames[FrameIndex]; }
            void Sync();
            uint32_t SyncFrame();
            void SetFrameInd() { FrameIndex = (FrameIndex + 1) % MAX_FRAMES; }
            long long Time;
            bool BeginFrame();
            void EndFrame();
            void EndRender();
            void StartRender(Gfx::InputAttachments inputs, AttachmentRules rules);

            void Draw(Gfx::RenderObject& object);
            void DrawThreaded(VkCommandBuffer cmd, Gfx::RenderObject& object, Material* mat,  Gfx::MeshInfo* mesh, Gfx::MeshPushConstants& constatns, bool ismodel, uint32_t inst_ind);
            void DrawMeshes(Gfx::RenderObject& object);
            void DrawMesh(Gfx::RenderObject& object, Gfx::MeshInfo* mesh, bool ismodel);
            void DrawSimple(Gfx::RenderObject& object);
            void NullTmpBindings();
            void CheckTmpBindings(Gfx::MeshInfo* mesh, Gfx::Material* material, bool* bindpipe, bool* bindindex);

        	FrameArray Frames;

            void BeginRendering(VkCommandBuffer cmd, const VkRenderingInfoKHR* renderinfo) { vkCmdBeginRenderingKHR(cmd, renderinfo); }
            void EndRendering(VkCommandBuffer cmd) { vkCmdEndRenderingKHR(cmd); }
            bool IsOnResize() const { return OnResize; }
            void SetOnResize(bool ison) { OnResize = ison; }

            int GetInstanceSize() const { return 20; }

            const glm::vec3 GetCameraPos() const { return glm::vec3(CamData.viewpos); }
            const glm::mat4& GetProjection() const { return CamData.proj; }
            const glm::mat4& GetView() const { return CamData.view; }

            void ResetInstanceInd() { InstanceIndex = 0; }
            void IncInstanceInd(int size) { InstanceIndex += size; }
            uint32_t GetInstanceInd() { return InstanceIndex; }// += size; }
            void SetInstanceInd(uint32_t i) { InstanceIndex = i; }// += size; }

            void SetUpdateSamplers(bool upd) { UpdateSamples = upd; }
            bool GetUpdateSamplers() const { return UpdateSamples; }

            void DebugRenderName(const std::string& str);
            void EndRenderName();

            VkCommandBufferAllocateInfo CommandBufferCreateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level) {

                VkCommandBufferAllocateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                info.pNext = nullptr;
            
                info.commandPool = pool;
                info.commandBufferCount = count;
                info.level = level;
                return info;
            }

            VkCommandBuffer GetCmd() { return Cmd.GetCmd(); }
            void SetCmd(VkCommandBuffer cmd) { Cmd.SetCmd(cmd); }
        private:
            RenderTarget* RTs[RT_SIZE];
            TexturesMap Textures;

            StorageData StorageBuffer;
            InstanceData InstanceBuffer;
            CameraData 	CamData;
            UploadContext Upload;

            uint32_t InstanceCount = 0;
            uint32_t InstanceIndex = 0;

            bool UpdateSamples = false;
            bool OnResize = false;
	        int FrameIndex = 0;
            uint32_t SwapchainIndex = 0;

            VkRenderingAttachmentInfoKHR AttachmentInfos[Gfx::RT_SIZE];
            PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
        	PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR{VK_NULL_HANDLE};

            Gfx::CommandBuffer Cmd;

            Gfx::Material* TmpBindMaterial = nullptr;
	        Gfx::MeshInfo* TmpBindMesh = nullptr;
    };
}
