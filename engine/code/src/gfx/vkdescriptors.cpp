#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkrenderer.h"

void Gfx::Descriptors::Init()
{
    CreateDescriptorPool();
    CreateDescriptorSets();
}

void Gfx::Descriptors::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 7> sizes{};
	sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES);
	sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES);
	sizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES);
	sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES);
	sizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sizes[4].descriptorCount = 1;
	sizes[5].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	sizes[5].descriptorCount = 1;
	sizes[6].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	sizes[6].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.maxSets = 100;
	poolinfo.poolSizeCount = (uint32_t)sizes.size();
	poolinfo.pPoolSizes = sizes.data();
	vkCreateDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &DescriptorPool);
}

void Gfx::Descriptors::CreateDescriptorSetLayout(VkDescriptorSetLayout descsetlayout, VkDescriptorSetLayoutBinding* binding, uint32_t bindingcount)
{
	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;
	setinfo.bindingCount = bindingcount; 
	setinfo.flags = 0;
	setinfo.pBindings = binding;
	vkCreateDescriptorSetLayout(Gfx::Device::GetInstance()->GetDevice(), &setinfo, nullptr, &descsetlayout);
}
VkDescriptorSetLayoutBinding DescSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
    VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stageFlags;
	return setbind;
}

void Gfx::Descriptors::AllocateDescriptorSet(VkDescriptorSetLayout setlayout, VkDescriptorSet desc)
{
    VkDescriptorSetAllocateInfo allocinfo{};
	allocinfo.pNext = nullptr;
	allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocinfo.descriptorPool = DescriptorPool;
	allocinfo.descriptorSetCount  = 1;
	allocinfo.pSetLayouts = &setlayout;

	VK_ASSERT(vkAllocateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), &allocinfo, &desc), "failed to allocate descriptor sets!");
}

VkWriteDescriptorSet WriteDescToBuffer(VkDescriptorType type, VkDescriptorSet dstSet,  uint32_t binding, uint32_t count)
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = count;
	write.descriptorType = type;

	return write;
}

void Gfx::Descriptors::UpdateAttachmentDescriptors()
{
    AllocateDescriptorSet(DescSetLayouts[DESC_SET_LAYOUT_SAMPLER], DescriptorSets[DESC_SET_LAYOUT_SAMPLER]);

	// std::array<VkDescriptorImageInfo, 2> descriptors{};
	// descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// descriptors[0].imageView = Gfx::Renderer::GetInstance()->GetMainRT()->GetImageView();
	// descriptors[0].sampler = VK_NULL_HANDLE;

	// descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// descriptors[1].imageView = Gfx::Renderer::GetInstance()->GetDepthRT()->GetImageView();
	// descriptors[1].sampler = VK_NULL_HANDLE;

	// std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	// for (size_t i = 0; i < 2; i++) {
	// 	writeDescriptorSets.push_back(WriteDescToBuffer(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, DescriptorSets[DESC_SET_LAYOUT_SAMPLER], static_cast<uint32_t>(i + 1), 1));
	// 	writeDescriptorSets[i].pImageInfo = &descriptors[i];
	// }
	// vkUpdateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
}

void Gfx::Descriptors::CreateDescriptorSets()
{
    // VkDescriptorSetLayoutBinding binding = { DescSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0) };
    // CreateDescriptorSetLayout(
    //     DescSetLayouts[DESC_SET_LAYOUT_GLOBAL],
    //     &binding,
    //     1
    // );
    
    // binding = { DescSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0) };
    // CreateDescriptorSetLayout(
    //     DescSetLayouts[DESC_SET_LAYOUT_STORAGE],
    //     &binding,
    //     1
    // );

	std::array<VkDescriptorSetLayoutBinding, 3> texturebinding = {DescSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0), DescSetLayoutBinding(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 1), DescSetLayoutBinding(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 2)};
    CreateDescriptorSetLayout(
        DescSetLayouts[DESC_SET_LAYOUT_SAMPLER],
        texturebinding.data(),
        texturebinding.size()
    );

    UpdateAttachmentDescriptors();
}
