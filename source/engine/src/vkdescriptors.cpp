
#include "../includes/vkdescriptors.h"

size_t DescriptorBuilder::paduniformbuffersize(size_t originalsize)
{
	size_t minUboAlignment = renderer.getdevice().minuniformbufferoffsetalignment;
	size_t alignedSize = originalsize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}


VkDescriptorSetLayoutBinding descriptorsetlayoutbinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stageFlags;

	return setbind;
}

VkWriteDescriptorSet writedescriptorbuffer(VkDescriptorType type, VkDescriptorSet dstSet,  uint32_t binding)
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;

	return write;
}


void DescriptorBuilder::builddescriptors() {
	BufferBuilder buffer;

	// std::vector<VkDescriptorPoolSize> sizes =
	// {
	// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 }
	// };
	std::array<VkDescriptorPoolSize, 2> sizes{};
	sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);;
	poolinfo.poolSizeCount = (uint32_t)sizes.size();
	poolinfo.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(renderer.getdevice().getlogicaldevice(), &poolinfo, nullptr, &descriptorpool);

	VkDescriptorSetLayoutBinding cambufferbinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutBinding samplerbinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { cambufferbinding, samplerbinding };

	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;

	setinfo.bindingCount = static_cast<uint32_t>(bindings.size()); 
	setinfo.flags = 0;
	setinfo.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(renderer.getdevice().getlogicaldevice(), &setinfo, nullptr, &globalsetlayout);
	
	deletorhandler.pushfunction([&]() {
		vkDestroyDescriptorSetLayout(renderer.getdevice().getlogicaldevice(), globalsetlayout, nullptr);
		vkDestroyDescriptorPool(renderer.getdevice().getlogicaldevice(), descriptorpool, nullptr);
	});

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, globalsetlayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorpool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	
	descriptorsets.resize(MAX_FRAMES_IN_FLIGHT);
	VK_ASSERT(vkAllocateDescriptorSets(renderer.getdevice().getlogicaldevice(), &allocInfo, descriptorsets.data()), "failed to allocate descriptor sets!");


	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		const size_t cambuffersize = MAX_FRAMES_IN_FLIGHT * paduniformbuffersize(sizeof(CameraData));
		buffer.createuniformbuffer(renderer, CameraBuffer[i], cambuffersize);

		VkDescriptorBufferInfo binfo;
		binfo.buffer = CameraBuffer[i].buffer;
		binfo.offset = 0;//paduniformbuffersize(sizeof(CameraData)) * i;//0;
		binfo.range = sizeof(CameraData);

		VkDescriptorImageInfo imageInfo{};
	    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	    imageInfo.imageView = texturehandler.gettexture("first")->imageview;
	    imageInfo.sampler = texturehandler.gettexture("first")->sampler;

		std::array<VkWriteDescriptorSet, 2> setwrite{};
		setwrite[0] = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorsets[i], 0);
		setwrite[0].pBufferInfo = &binfo;
		
		setwrite[1] = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorsets[i],1);
		setwrite[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(renderer.getdevice().getlogicaldevice(), static_cast<uint32_t>(setwrite.size()), setwrite.data(), 0, nullptr);
	}
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		deletorhandler.pushfunction([=]() {
	        vkDestroyBuffer(renderer.getdevice().getlogicaldevice(), CameraBuffer[i].buffer, nullptr);
	        vkFreeMemory(renderer.getdevice().getlogicaldevice(), CameraBuffer[i].devicememory, nullptr);
	    });
	}
}