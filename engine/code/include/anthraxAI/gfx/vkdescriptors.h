#pragma once

#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"

#include "anthraxAI/gfx/bufferhelper.h"

#include <unordered_map>
#include <algorithm>

#define MAX_BINDING 3

static constexpr uint32_t UniformBinding = 0;
static constexpr uint32_t StorageBinding = 1;
static constexpr uint32_t TextureBinding = 2;

namespace Gfx
{
    enum DescriptorSetLayoutEnum {
        DESC_SET_LAYOUT_GLOBAL = 0,
        DESC_SET_LAYOUT_SAMPLER,
        DESC_SET_LAYOUT_STORAGE,
        DESC_SET_LAYOUT_TRANSFORMS
    };

    class DescriptorsBase : public Utils::Singleton<DescriptorsBase>
    {
        public:
            void Init();
            void CleanAll();
            void CleanBindless();

            void AllocateBuffers();

            VkDeviceMemory GetCameraBufferMemory() const { return CameraBuffer.DeviceMemory; }
            VkBuffer GetCameraBuffer() const { return CameraBuffer.Buffer; }
            BufferHelper::Buffer& GetCameraUBO() { return CameraBuffer; }

            VkBuffer GetStorageBuffer() const { return StorageBuffer.Buffer; }
            BufferHelper::Buffer& GetStorageUBO() { return StorageBuffer; }
            VkDeviceMemory GetStorageBufferMemory() const { return StorageBuffer.DeviceMemory; }

            VkBuffer GetInstanceBuffer() const { return InstanceBuffer.Buffer; }
            BufferHelper::Buffer& GetInstanceUBO() { return InstanceBuffer; }
            VkDeviceMemory GetInstanceBufferMemory() const { return InstanceBuffer.DeviceMemory; }

            size_t PadUniformBufferSize(size_t originalsize);

            uint32_t UpdateTexture(VkImageView imageview, VkSampler sampler);
            uint32_t UpdateBuffer(VkBuffer buffer, VkBufferUsageFlagBits usage);

            VkDescriptorSet* GetBindlessSet() { return &BindlessDescriptor; }
            VkDescriptorSetLayout GetBindlessLayout() { return BindlessLayout; }

        private:
            void AllocateDataBuffers();
            void AllocateStorageBuffers();

            BufferHelper::Buffer CameraBuffer;
            BufferHelper::Buffer StorageBuffer;
            BufferHelper::Buffer InstanceBuffer;

            VkDescriptorPool Pool;
	        VkDescriptorSetLayout BindlessLayout = VK_NULL_HANDLE;
            VkDescriptorSet BindlessDescriptor;

            uint32_t TextureHandle = 0;
            uint32_t BufferHandle = 0;

            BufferHelper::Buffer BindlessBuffer;
    };
}
