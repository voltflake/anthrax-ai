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
            void CreateCommands();
            void CreateRenderTargets();

            void CleanResources();
            
            void CreateTextures();
            RenderTarget CreateTexture(const std::string& path);
            void CreateSampler(RenderTarget& rt);

            TexturesMap GetTextureMap() const { return Textures; }
            RenderTarget* GetTexture(const std::string& path);
            RenderTarget* GetMainRT() { return MainRT; }
            RenderTarget* GetDepthRT() { return DepthRT; }

            void PrepareCameraBuffer(Core::Camera& camera);
            void PrepareStorageBuffer();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            VkRenderingAttachmentInfoKHR* GetAttachmentInfo(AttachmentFlags flag);

            int GetFrameInd() { return FrameIndex; }
            FrameData& GetFrame() { return Frames[FrameIndex]; }
            void Sync();
            uint32_t SyncFrame();
            void SetFrameInd() { FrameIndex = (FrameIndex + 1) % MAX_FRAMES; }

            void StartFrame(AttachmentFlags attachmentflags);
            void EndFrame();

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

            int GetSelectedID() const { return SelectedID; }
        private:
            RenderTarget* DepthRT;
            RenderTarget* MainRT;

            TexturesMap Textures;
	        
            u_int SelectedID = 0;
            StorageData StorageBuffer;
            CameraData 	CamData;
            UploadContext Upload;
            
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
