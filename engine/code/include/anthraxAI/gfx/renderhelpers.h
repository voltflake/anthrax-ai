#pragma once
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/model.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include <cstdint>
namespace Gfx
{
    enum ObjectTypes {
        OBJECT_DEF = 0,
        OBJECT_GIZMO = 1000,
    };

    enum BindlessDataType {
        BINDLESS_DATA_NONE = 0,
        BINDLESS_DATA_TEXTURE,
        BINDLESS_DATA_CAM_BUFFER,
        BINDLESS_DATA_SIZE
    };
   
    struct RenderObject {
	    Gfx::MeshInfo* Mesh = nullptr;
	    Gfx::ModelInfo* Model = nullptr;
	    Gfx::Material* Material = nullptr;
      Gfx::RenderTarget* Texture = nullptr;

      uint32_t BindlessOffset = 0;    
	    Vector3<float> Position;

        bool VertexBase = false;
        bool IsGrid = false;
        bool IsVisible = true;
        bool HasStorage = false;
        
        uint32_t ID;
        uint32_t GizmoID;

        uint32_t BufferBind;
        uint32_t StorageBind;
        uint32_t TextureBind;
    };
   
    #define DEPTH_ARRAY_SCALE 512
    #define MAX_BONES 200
    #define BONE_ARRAY_SIZE (sizeof(glm::mat4) * MAX_BONES)
    
    struct StorageData {
        glm::mat4 bonesmatrices[MAX_BONES];
        u_int data[DEPTH_ARRAY_SCALE] = {0};
    };

    struct CameraData {
        glm::vec4 viewpos;
        glm::vec4 mousepos;
        glm::vec4 viewport;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 viewproj;

        float time;
        float p0;
        float p1;
        float p2;
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

    
    struct BasicParams {
        uint32_t camerabufer = 0;
        uint32_t texturehandle = 0;
        uint32_t storagebuffer = 0;
        uint32_t pad0;
    };
    struct CamBufferParams {
        uint32_t camerabufer = 0;
        uint32_t pad0;
        uint32_t pad1;
        uint32_t pad2;
    };

}
