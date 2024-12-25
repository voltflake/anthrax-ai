#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/bufferhelper.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkcmdhelper.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"

namespace Gfx
{
    class Renderer : public Utils::Singleton<Renderer> 
    {
        public:
            void CleanTextures();

            void CreateCommands();
            void CreateRenderTargets();

            void CleanResources();
            
            void CreateTextures();
            bool CreateTextureFromInfo(const std::string& texturename);
            RenderTarget CreateTexture(const std::string& path);
            void CreateSampler(RenderTarget& rt);

            TexturesMap GetTextureMap() const { return Textures; }
            RenderTarget* GetTexture(const std::string& path);
            RenderTarget* GetMainRT() { return MainRT; }
            RenderTarget* GetDepthRT() { return DepthRT; }

            void PrepareCameraBuffer(Keeper::Camera& camera);
            void PrepareInstanceBuffer();
            void PrepareStorageBuffer();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);
            
            VkRenderingAttachmentInfoKHR* GetAttachmentInfo(AttachmentFlags flag, AttachmentFlags loadop = Gfx::AttachmentFlags::RENDER_ATTACHMENT_CLEAR);

            void RenderUI();

            int GetFrameInd() { return FrameIndex; }
            FrameData& GetFrame() { return Frames[FrameIndex]; }
            void Sync();
            uint32_t SyncFrame();
            void SetFrameInd() { FrameIndex = (FrameIndex + 1) % MAX_FRAMES; }

            void BeginFrame();
            void EndFrame();
            void EndRender();
            void StartRender(AttachmentFlags attachmentflags);

            void Draw(Gfx::RenderObject& object);
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

        private:
            RenderTarget* DepthRT;
            RenderTarget* MainRT;

            TexturesMap Textures;
	        
            StorageData StorageBuffer;
            InstanceData InstanceBuffer;
            CameraData 	CamData;
            UploadContext Upload;
            
            uint32_t InstanceCount = 0;
            uint32_t InstanceIndex = 0;

            bool OnResize = false;
	        int FrameIndex = 0;
            uint32_t SwapchainIndex = 0;
            
            VkRenderingAttachmentInfoKHR AttachmentInfos[RENDER_ATTACHMENT_SIZE];
            PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
        	PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR{VK_NULL_HANDLE};

            Gfx::CommandBuffer Cmd;

            Gfx::Material* TmpBindMaterial = nullptr;
	        Gfx::MeshInfo* TmpBindMesh = nullptr;
    };
}
