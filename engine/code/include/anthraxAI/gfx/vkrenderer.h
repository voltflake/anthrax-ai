#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/bufferhelper.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkcmdhelper.h"

namespace Gfx
{
    struct CameraData {
        glm::vec4 viewpos;
        glm::vec4 mousepos;
        glm::vec4 viewport;

        // glm::vec4 dir_light_pos = glm::vec4(0.63f, 0.82f, 0.48f, 1);
        // glm::vec4 dir_light_dir = glm::vec4(1);
        // glm::vec4 dir_light_color = glm::vec4(-2.2f, 1.0f, 2.0f, 1);

        // glm::vec4 point_light_pos[MAX_POINT_LIGHTS] = { glm::vec4(1.0) };
        // glm::vec4 point_light_dir[MAX_POINT_LIGHTS] = { glm::vec4(1) };
        // glm::vec4 point_light_color[MAX_POINT_LIGHTS] = { glm::vec4(1.0) };

        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 viewproj;

        // float ambient = 0.1;
        // float diffuse = 0.5;
        // float specular = 0.5;

        // bool hasdirectional = 1;
        // int pointlightamount;
    };

    struct FrameData {
        DescriptorAllocator* DynamicDescAllocator;
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

            void CleanResources();
            
            void CreateTextures();
            RenderTarget CreateTexture(const std::string& path);
            void CreateSampler(RenderTarget& rt);

            RenderTarget& GetTexture(const std::string& path) { return Textures[path]; }
            RenderTarget* GetMainRT() { return MainRT; }
            RenderTarget* GetDepthRT() { return DepthRT; }

            void CopyToSwapchain(VkCommandBuffer cmd, RenderTarget* rt, uint32_t swapchainimageindex);
            void PrepareCameraBuffer();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            void Sync();

            int GetFrameInd() { return FrameIndex; }
            FrameData& GetFrame() { return Frames[FrameIndex]; }
            uint32_t SyncFrame();
            void SetFrameInd() { FrameIndex = (FrameIndex + 1) % MAX_FRAMES; }
            void FinalizeRendering(CommandBuffer& cmd);

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

            PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
        	PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR{VK_NULL_HANDLE};

    };
}