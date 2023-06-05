
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

	std::array<VkDescriptorPoolSize, 3> sizes{};
	sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.maxSets = 10;
	poolinfo.poolSizeCount = (uint32_t)sizes.size();
	poolinfo.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(renderer.getdevice().getlogicaldevice(), &poolinfo, nullptr, &descriptorpool);

	VkDescriptorSetLayoutBinding cambufferbinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	//VkDescriptorSetLayoutBinding samplerbinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	
	VkDescriptorSetLayoutBinding bindings = { cambufferbinding };

	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;

	setinfo.bindingCount = 1; 
	setinfo.flags = 0;
	setinfo.pBindings = &bindings;

	vkCreateDescriptorSetLayout(renderer.getdevice().getlogicaldevice(), &setinfo, nullptr, &globalsetlayout);
	
	VkDescriptorSetLayoutBinding texturebind = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

	VkDescriptorSetLayoutCreateInfo set3info = {};
	set3info.bindingCount = 1;
	set3info.flags = 0;
	set3info.pNext = nullptr;
	set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set3info.pBindings = &texturebind;

	vkCreateDescriptorSetLayout(renderer.getdevice().getlogicaldevice(), &set3info, nullptr, &singletexturesetlayout);

	deletorhandler.pushfunction([&]() {
		vkDestroyDescriptorSetLayout(renderer.getdevice().getlogicaldevice(), singletexturesetlayout, nullptr);
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

		VkWriteDescriptorSet setwrite;
		setwrite = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorsets[i], 0);
		setwrite.pBufferInfo = &binfo;
		
	
		vkUpdateDescriptorSets(renderer.getdevice().getlogicaldevice(), 1, &setwrite, 0, nullptr);

	}

	
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		deletorhandler.pushfunction([=]() {
	        vkDestroyBuffer(renderer.getdevice().getlogicaldevice(), CameraBuffer[i].buffer, nullptr);
	        vkFreeMemory(renderer.getdevice().getlogicaldevice(), CameraBuffer[i].devicememory, nullptr);
	    });
	}
}


void DescriptorBuilder::updatesamplerdescriptors(std::string texture) {

	VkDescriptorSet dsset;

	VkDescriptorSetAllocateInfo allocInfo2{};
	allocInfo2.pNext = nullptr;
	allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo2.descriptorPool = descriptorpool;
	allocInfo2.descriptorSetCount  = 1;
	allocInfo2.pSetLayouts = &singletexturesetlayout;

	VK_ASSERT(vkAllocateDescriptorSets(renderer.getdevice().getlogicaldevice(), &allocInfo2, &dsset), "failed to allocate descriptor sets!");

	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.sampler = texturehandler.gettexture(texture)->sampler;
	imageBufferInfo.imageView = texturehandler.gettexture(texture)->imageview;
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet texture1 = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (dsset), 0);
	texture1.pImageInfo = &imageBufferInfo;

	vkUpdateDescriptorSets(renderer.getdevice().getlogicaldevice(), 1, &texture1, 0, nullptr);

	//textureset.insert(textureset.begin(), dsset);
	if (textureset.empty()){
		textureset.reserve(2);
	}
	textureset.push_back(dsset);
}
