#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/bufferhelper.h"

namespace Gfx
{
    struct FrameData {
        VkSemaphore PresentSemaphore, RenderSemaphore;
        VkFence RenderFence;	
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
    };

    struct UploadContext {
        VkFence UploadFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

    typedef std::array<FrameData, MAX_FRAMES> FrameArray;
    typedef std::unordered_map<std::string, RenderTarget> TexturesMap;

    class Renderer : public Utils::Singleton<Renderer> 
    {
        public:
            void CreateCommands();
            void CreateRenderTargets();
            
            void CreateTextures();
            RenderTarget CreateTexture(const std::string& path);
            void CreateSampler(RenderTarget& rt);

            RenderTarget& GetRT(const std::string& path) { return Textures[path]; }
            RenderTarget* GetMainRT() { return MainRT; }
            RenderTarget* GetDepthRT() { return DepthRT; }

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            void Sync();
        private:
            RenderTarget* DepthRT;
            RenderTarget* MainRT;

            TexturesMap Textures;

        	FrameArray Frames;
            UploadContext Upload;
    };
}