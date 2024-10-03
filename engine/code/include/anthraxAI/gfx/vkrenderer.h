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

            void PrepareCameraBuffer();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

           VkRenderingAttachmentInfoKHR* GetAttachmentInfo(AttachmentFlags flag);

            int GetFrameInd() { return FrameIndex; }
            FrameData& GetFrame() { return Frames[FrameIndex]; }
            void Sync();
            uint32_t SyncFrame();
            void SetFrameInd() { FrameIndex = (FrameIndex + 1) % MAX_FRAMES; }

            void StartFrame(AttachmentFlags attachmentflags);
            void EndFrame();

            void Draw(Gfx::RenderObject& object, bool bindpipe, bool bindindex);
            void DrawSimple(Gfx::RenderObject& object);

        	FrameArray Frames;

            void BeginRendering(VkCommandBuffer cmd, const VkRenderingInfoKHR* renderinfo) { vkCmdBeginRenderingKHR(cmd, renderinfo); }
            void EndRendering(VkCommandBuffer cmd) { vkCmdEndRenderingKHR(cmd); }

        private:
            RenderTarget* DepthRT;
            RenderTarget* MainRT;

            TexturesMap Textures;
	        
            CameraData 	camdata;
            UploadContext Upload;

	        int FrameIndex = 0;
            uint32_t SwapchainIndex = 0;
            
            VkRenderingAttachmentInfoKHR AttachmentInfos[RENDER_ATTACHMENT_SIZE];
            PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
        	PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR{VK_NULL_HANDLE};

            Gfx::CommandBuffer Cmd;
    };
}