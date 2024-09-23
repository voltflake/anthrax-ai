#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"

#include "anthraxAI/gfx/bufferhelper.h"

#include <unordered_map>
#include <algorithm>

namespace Gfx
{
    enum DescriptorSetLayoutEnum {
        DESC_SET_LAYOUT_GLOBAL = 0,
        DESC_SET_LAYOUT_SAMPLER,
        DESC_SET_LAYOUT_STORAGE,
        DESC_SET_LAYOUT_TRANSFORMS
    };
    
    class DescriptorAllocator
    {
        public:
            struct PoolSizes {
                std::vector<std::pair<VkDescriptorType,float>> Sizes =
                { // multiplier of the number of descriptor sets allocated for the pools
                    { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.f },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1.f },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.f },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1.f },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
                };
		    };

            void ResetPools();
            bool Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);
            void CleanUp();

        private:
            VkDescriptorPool CreatePool(int count, const DescriptorAllocator::PoolSizes& poolsizes);
            VkDescriptorPool GrabPool();

            VkDescriptorPool Pool{VK_NULL_HANDLE};
            PoolSizes DescriptorPoolSizes;
            std::vector<VkDescriptorPool> UsedPools;
            std::vector<VkDescriptorPool> FreePools;
    };

    class DescriptorLayoutCache {
        public:
            struct DescriptorLayoutInfo {
                std::vector<VkDescriptorSetLayoutBinding> Bindings;
                bool operator==(const DescriptorLayoutInfo& other) const;
                size_t Hash() const;
            };
            void CleanUp();
            VkDescriptorSetLayout CreateDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info);
        private:
            struct DescriptorLayoutHash {
                std::size_t operator()(const DescriptorLayoutInfo& k) const{
                    return k.Hash();
                }
            };
            std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> LayoutCache;
    };
    
    class Descriptors
    {
        public:
            static Descriptors Begin(DescriptorLayoutCache* layoutcache, DescriptorAllocator* allocator );

            Descriptors& BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferinfo, VkDescriptorType type, VkShaderStageFlags stageflags);
            Descriptors& BindImage(uint32_t binding, VkDescriptorImageInfo* imageinfo, VkDescriptorType type, VkShaderStageFlags stageflags);

            bool Build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
            bool Build(VkDescriptorSet& set);
        private:
            std::vector<VkWriteDescriptorSet> Writes;
            std::vector<VkDescriptorSetLayoutBinding> Bindings;

            DescriptorLayoutCache* Cache;
            DescriptorAllocator* Allocator;
    };
    
    typedef std::array<BufferHelper::Buffer, MAX_FRAMES> UboArray;

    class DescriptorsBase : public Utils::Singleton<DescriptorsBase>
    {
        public:
            void Init();
            void CleanUp();

            VkDeviceMemory GetCameraBufferMemory(int ind) { return CameraBuffer[ind].DeviceMemory; }
            VkBuffer GetCameraBuffer(int ind) { return CameraBuffer[ind].Buffer; }

            VkDescriptorSetLayout& GetTextureLayout() { return TextureSetLayout; }
            VkDescriptorSetLayout& GetGlobalLayout() { return GlobalSetLayout; }
            size_t PadUniformBufferSize(size_t originalsize);

            DescriptorAllocator* GetAllocator() { return Allocator; }
            DescriptorLayoutCache* GetLayoutCache() { return LayoutCache; }
        private:
            VkDescriptorSetLayout TextureSetLayout;
            VkDescriptorSetLayout GlobalSetLayout;

            UboArray CameraBuffer;

            DescriptorAllocator* Allocator;
	        DescriptorLayoutCache* LayoutCache;

    };
}