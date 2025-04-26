#pragma once
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/model.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Gfx
{
    enum BindlessDataType {
        BINDLESS_DATA_NONE = 0,
        BINDLESS_DATA_CAM_STORAGE_SAMPLER,
        BINDLESS_DATA_CAM_BUFFER,
        BINDLESS_DATA_SIZE
    };
    struct Material {
        VkPipelineLayout PipelineLayout;
        VkPipeline Pipeline;
    };

    struct RenderObject {
	    Gfx::MeshInfo* Mesh = nullptr;
	    Gfx::ModelInfo* Model[MAX_FRAMES] = { nullptr };
	    Gfx::Material* Material = nullptr;
        Gfx::RenderTarget* Texture = nullptr;
        std::vector<Gfx::RenderTarget*> Textures;

        std::string MaterialName;
        std::string TextureName;

	    Vector3<float> Position;

        bool VertexBase = false;
        bool IsGrid = false;
        bool HasStorage = false;

        bool IsVisible = true;
        uint32_t ID;
        bool IsSelected = false;

        uint32_t GizmoType;

        uint32_t BufferBind[MAX_FRAMES];
        uint32_t StorageBind[MAX_FRAMES];
        uint32_t InstanceBind[MAX_FRAMES];
        uint32_t TextureBind[MAX_FRAMES];
    };

    #define DEPTH_ARRAY_SCALE 512
    #define MAX_BONES 200
    #define MAX_INSTANCES 1000
    #define INSTANCES_ARRAY_SIZE (sizeof(glm::mat4) * MAX_INSTANCES)
    #define BONE_ARRAY_SIZE (sizeof(glm::mat4) * MAX_BONES)

    struct StorageData {
        u_int data[DEPTH_ARRAY_SCALE] = {0};
    };
    struct InstanceData {
        glm::mat4 bonesmatrices[MAX_BONES];
        glm::mat4 rendermatrix;

        uint32_t hasanimation = 0;
        uint32_t pad0 = 0;
        uint32_t pad1 = 0;
        uint32_t pad2 = 0;
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

    struct SecondaryCmdInfo {
        VkCommandBuffer Cmd;
        VkCommandPool Pool;
    };
    struct FrameData {
        VkSemaphore PresentSemaphore, RenderSemaphore;
        VkFence RenderFence;
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;
        VkCommandBuffer TracyVkCommandBuffer;
        std::vector<SecondaryCmdInfo> SecondaryCmd;
    };

    struct UploadContext {
        VkFence UploadFence;
        VkCommandPool CommandPool;
        VkCommandBuffer CommandBuffer;
    };

    class InputAttachments
    {
        public:
            void Add(Gfx::RenderTargetsList id, bool isdepth = false) { if (!isdepth) { Color = id; } else { Depth = id; } }
            void AddA(Gfx::RenderTargetsList id) { Albedo = id;  }
            void AddP(Gfx::RenderTargetsList id) { Position = id;  }
            void AddN(Gfx::RenderTargetsList id) { Normal = id;  }
            Gfx::RenderTargetsList GetColor() const { return Color; }
            Gfx::RenderTargetsList GetDepth() const { return Depth; }
            bool HasColor() const { return Color != Gfx::RT_SIZE; }
            bool HasAlbedo() const { return Albedo != Gfx::RT_SIZE; }
            bool HasPosition() const { return Position != Gfx::RT_SIZE; }
            bool HasNormal() const { return Normal != Gfx::RT_SIZE; }
            bool HasDepth() const { return Depth != Gfx::RT_SIZE; }
            bool IsColor(Gfx::RenderTargetsList id) { return id != Depth; }
            bool IsDepth(Gfx::RenderTargetsList id) { return id != Color; }
        private:
            Gfx::RenderTargetsList Color = Gfx::RT_SIZE;
            Gfx::RenderTargetsList Normal = Gfx::RT_SIZE;
            Gfx::RenderTargetsList Position = Gfx::RT_SIZE;
            Gfx::RenderTargetsList Albedo = Gfx::RT_SIZE;
            Gfx::RenderTargetsList Depth = Gfx::RT_SIZE;
    };

    enum AttachmentRules {
        ATTACHMENT_RULE_DONT_CARE = 1 << 0,
        ATTACHMENT_RULE_LOAD  = 1 << 1,
        ATTACHMENT_RULE_CLEAR = 1 << 2,
    };

    typedef std::array<FrameData, MAX_FRAMES> FrameArray;
    typedef std::unordered_map<std::string, RenderTarget> TexturesMap;


    struct BasicParams {
        uint32_t camerabuffer = 0;
        uint32_t texturehandle = 0;
        uint32_t storagebuffer = 0;
        uint32_t instancebuffer = 0;
    };
    struct CamBufferParams {
        uint32_t camerabuffer = 0;
        uint32_t pad0;
        uint32_t pad1;
        uint32_t pad2;
    };

}
