#pragma once
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/model.h"
namespace Gfx
{
    struct RenderObject {
	    Gfx::MeshInfo* Mesh = nullptr;
	    Gfx::ModelInfo* Model = nullptr;
	    Gfx::Material* Material = nullptr;
        Gfx::RenderTarget* Texture = nullptr;

        uint32_t BindlessOffset = 0;    
	    Vector3<float> Position;

        bool VertexBase = false;

        uint32_t BufferBind;
        uint32_t TextureBind;
    };

    struct CameraData {
        glm::vec4 viewpos;
        glm::vec4 mousepos;
        glm::vec4 viewport;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 viewproj;
    };

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

    enum AttachmentFlags {
        RENDER_ATTACHMENT_NONE  = 0,
        RENDER_ATTACHMENT_COLOR = 1 << 0,
        RENDER_ATTACHMENT_DEPTH = 1 << 1,
        RENDER_ATTACHMENT_SIZE
    };

    typedef std::array<FrameData, MAX_FRAMES> FrameArray;
    typedef std::unordered_map<std::string, RenderTarget> TexturesMap;

    enum BindlessDataType {
        BINDLESS_DATA_NONE = 0,
        BINDLESS_DATA_TEXTURE,
        BINDLESS_DATA_SIZE
    };

    struct TextureParams {
        uint32_t camerabufer = 0;
        uint32_t texturehandle = 0;
        uint32_t pad0;
        uint32_t pad1;
    };
}