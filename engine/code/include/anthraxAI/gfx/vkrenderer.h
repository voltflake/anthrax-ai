#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/bufferhelper.h"
#include "anthraxAI/gfx/vkdescriptors.h"

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

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            void Sync();

        	FrameArray Frames;

        private:
            RenderTarget* DepthRT;
            RenderTarget* MainRT;

            TexturesMap Textures;

            UploadContext Upload;
    };
}