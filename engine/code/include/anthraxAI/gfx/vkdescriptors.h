#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include <unordered_map>
namespace Gfx
{
    enum DescriptorSetLayoutEnum {
        DESC_SET_LAYOUT_GLOBAL = 0,
        DESC_SET_LAYOUT_SAMPLER,
        DESC_SET_LAYOUT_STORAGE,
        DESC_SET_LAYOUT_TRANSFORMS
    };
    typedef std::unordered_map<DescriptorSetLayoutEnum, VkDescriptorSetLayout> DescriptorSetLayoutMap;
    typedef std::unordered_map<DescriptorSetLayoutEnum, VkDescriptorSet> DescriptorSetMap;
    
    class Descriptors : public Utils::Singleton<Descriptors>
    {
        public:
            void Init();
        private:
            void CreateDescriptorPool();
            void CreateDescriptorSets();
            void CreateDescriptorSetLayout(VkDescriptorSetLayout descsetlayout, VkDescriptorSetLayoutBinding* binding, uint32_t bindingcount);
            
            void AllocateDescriptorSet(VkDescriptorSetLayout setlayout, VkDescriptorSet desc);
            void UpdateAttachmentDescriptors();

            VkDescriptorSetLayout dummysetlayout;
            VkDescriptorSet dummyset;
            DescriptorSetLayoutMap DescSetLayouts;
            DescriptorSetMap DescriptorSets;

        	VkDescriptorPool DescriptorPool;

    };
}