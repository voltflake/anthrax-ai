#pragma once

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
 
    struct Range {
        uint32_t offset;
        uint32_t size;
        void* data;
    };

    class DescriptorsBase : public Utils::Singleton<DescriptorsBase>
    {
        public:
            void Init();
            void CleanUp();

            void AllocateDataBuffers();

            VkDeviceMemory GetCameraBufferMemory() const { return CameraBuffer.DeviceMemory; }
            VkBuffer GetCameraBuffer() const { return CameraBuffer.Buffer; }
            BufferHelper::Buffer GetCameraUBO() const { return CameraBuffer; }

            size_t PadUniformBufferSize(size_t originalsize);

            uint32_t UpdateTexture(VkImageView imageview, VkSampler sampler);
            uint32_t UpdateBuffer(VkBuffer buffer, VkBufferUsageFlagBits usage);

            template<class TData>
            uint32_t AddRange(TData&& data) {
                size_t datasize = sizeof(TData);
                auto* bytes = new TData;
                *bytes = data;

                uint32_t curoffset = LastOffset;
                Ranges.push_back({ curoffset, static_cast<uint32_t>(datasize), bytes });

                LastOffset += PadUniformBufferSize(datasize);
                return curoffset;
            }

            void Build();

            VkDescriptorSet* GetBindlessSet() { return &BindlessDescriptor; }
            VkDescriptorSetLayout GetBindlessLayout() { return BindlessLayout; }

            VkDescriptorSet* GetDescriptorSet() { return &GlobalDescriptor; }
            VkDescriptorSetLayout GetDescriptorSetLayout() { return GlobalLayout; }

        private:
            VkDescriptorSet GlobalDescriptor;
            VkDescriptorSetLayout GlobalLayout;

            BufferHelper::Buffer CameraBuffer;

            VkDescriptorPool Pool;
	        VkDescriptorSetLayout BindlessLayout = VK_NULL_HANDLE;
            VkDescriptorSet BindlessDescriptor;

            uint32_t TextureHandle = 0;
            uint32_t BufferHandle = 0;

            uint32_t LastOffset = 0;
            std::vector<Range> Ranges;
            BufferHelper::Buffer BindlessBuffer;
    };
}