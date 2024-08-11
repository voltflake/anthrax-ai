
#include "anthraxAI/vkdescriptors.h"

void DescriptorBuilder::cleartextureset() {
	vkDestroyDescriptorSetLayout(renderer.getdevice()->getlogicaldevice(), singletexturesetlayout, nullptr);
	vkDestroyDescriptorSetLayout(renderer.getdevice()->getlogicaldevice(), storagesetlayout, nullptr);
	vkDestroyDescriptorSetLayout(renderer.getdevice()->getlogicaldevice(), globalsetlayout, nullptr);
	vkDestroyDescriptorPool(renderer.getdevice()->getlogicaldevice(), descriptorpool, nullptr);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	    vkDestroyBuffer(renderer.getdevice()->getlogicaldevice(), CameraBuffer[i].buffer, nullptr);
	    vkFreeMemory(renderer.getdevice()->getlogicaldevice(), CameraBuffer[i].devicememory, nullptr);

		vkDestroyBuffer(renderer.getdevice()->getlogicaldevice(), StorageBuffer[i].buffer, nullptr);
	    vkFreeMemory(renderer.getdevice()->getlogicaldevice(), StorageBuffer[i].devicememory, nullptr);
	}

	descriptorsets.clear();
	storagedescsets.clear();
	textureset.clear();
}

size_t DescriptorBuilder::paduniformbuffersize(size_t originalsize)
{
	size_t minUboAlignment = renderer.getdevice()->minuniformbufferoffsetalignment;
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

VkWriteDescriptorSet writedescriptorbuffer(VkDescriptorType type, VkDescriptorSet dstSet,  uint32_t binding, uint32_t count)
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


void DescriptorBuilder::builddescriptors() {
	BufferBuilder buffer;
	BufferBuilder storagebuffer;

	std::array<VkDescriptorPoolSize, 6> sizes{};
	sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sizes[3].descriptorCount = 2;
	sizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sizes[4].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[5].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	sizes[5].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	sizes[6].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	sizes[6].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.maxSets = 100;
	poolinfo.poolSizeCount = (uint32_t)sizes.size();
	poolinfo.pPoolSizes = sizes.data();
	vkCreateDescriptorPool(device.getlogicaldevice(), &poolinfo, nullptr, &descriptorpool);


// create wrappers for this
	VkDescriptorSetLayoutBinding cambufferbinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutBinding bindings = { cambufferbinding };
	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;
	setinfo.bindingCount = 1; 
	setinfo.flags = 0;
	setinfo.pBindings = &bindings;
	vkCreateDescriptorSetLayout(device.getlogicaldevice(), &setinfo, nullptr, &globalsetlayout);

	VkDescriptorSetLayoutBinding storagebinding = descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutBinding storagebindings = { storagebinding };
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;
	setinfo.bindingCount = 1; 
	setinfo.flags = 0;
	setinfo.pBindings = &storagebindings;
	vkCreateDescriptorSetLayout(device.getlogicaldevice(), &setinfo, nullptr, &storagesetlayout);
	
	std::array<VkDescriptorSetLayoutBinding, 3> texturebind = {descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0), descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 1), descriptorsetlayoutbinding(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 2)};
	VkDescriptorSetLayoutCreateInfo set3info = {};
	set3info.bindingCount = 3; //size of VkDescriptorSetLayoutBinding
	set3info.flags = 0;
	set3info.pNext = nullptr;
	set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set3info.pBindings = texturebind.data();
	vkCreateDescriptorSetLayout(device.getlogicaldevice(), &set3info, nullptr, &singletexturesetlayout);

	updateattachmentdescriptors(device);

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, globalsetlayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorpool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	descriptorsets.resize(MAX_FRAMES_IN_FLIGHT);
	VK_ASSERT(vkAllocateDescriptorSets(device.getlogicaldevice(), &allocInfo, descriptorsets.data()), "failed to allocate descriptor sets!");
	
	std::vector<VkDescriptorSetLayout> storagelayouts(MAX_FRAMES_IN_FLIGHT, storagesetlayout);
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorpool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = storagelayouts.data();
	storagedescsets.resize(MAX_FRAMES_IN_FLIGHT);
	VK_ASSERT(vkAllocateDescriptorSets(device.getlogicaldevice(), &allocInfo, storagedescsets.data()), "failed to allocate descriptor sets!");

	//copy initial storage buffer values
    VkDeviceSize bufferSize = sizeof(StorageData);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		// UBO bffer
		const size_t cambuffersize = MAX_FRAMES_IN_FLIGHT * paduniformbuffersize(sizeof(CameraData));
		buffer.createbuffer(renderer, CameraBuffer[i], cambuffersize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		VkDescriptorBufferInfo binfo;
		binfo.buffer = CameraBuffer[i].buffer;
		binfo.offset = 0;//paduniformbuffersize(sizeof(CameraData)) * i;//0;
		binfo.range = sizeof(CameraData);

		VkWriteDescriptorSet setwrite;
		setwrite = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorsets[i], 0, 1);
		setwrite.pBufferInfo = &binfo;

		storagebuffer.createbuffer(renderer, StorageBuffer[i], bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		
		VkDescriptorBufferInfo storageinfo;
		storageinfo.buffer = StorageBuffer[i].buffer;
		storageinfo.offset = 0;
		storageinfo.range = sizeof(StorageData);

		VkWriteDescriptorSet setwritestorage;
		setwritestorage = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, storagedescsets[i], 0, 1);
		setwritestorage.pBufferInfo = &storageinfo;

		VkWriteDescriptorSet setwrites[] = { setwrite, setwritestorage };
		vkUpdateDescriptorSets(device.getlogicaldevice(), 2, setwrites, 0, nullptr);
	}
}

void DescriptorBuilder::updateattachmentdescriptors(DeviceBuilder& dev)
{
	VkDescriptorSetAllocateInfo allocInfo2{};
	allocInfo2.pNext = nullptr;
	allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo2.descriptorPool = descriptorpool;
	allocInfo2.descriptorSetCount  = 1;
	allocInfo2.pSetLayouts = &singletexturesetlayout;

	VK_ASSERT(vkAllocateDescriptorSets(dev.getlogicaldevice(), &allocInfo2, &attachmentset), "failed to allocate descriptor sets!");

	std::array<VkDescriptorImageInfo, 2> descriptors{};
	descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptors[0].imageView = dev.mainrendertarget.texture->imageview;
	descriptors[0].sampler = VK_NULL_HANDLE;

	descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptors[1].imageView = dev.depthimage.texture->imageview;
	descriptors[1].sampler = VK_NULL_HANDLE;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	for (size_t i = 0; i < 2; i++) {
		writeDescriptorSets.push_back(writedescriptorbuffer(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, (attachmentset), static_cast<uint32_t>(i + 1), 1));
		writeDescriptorSets[i].pImageInfo = &descriptors[i];
	}

	vkUpdateDescriptorSets(dev.getlogicaldevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
}

void DescriptorBuilder::updatesamplerdescriptors(std::string texture) {

	VkDescriptorSet dsset;

	VkDescriptorSetAllocateInfo allocInfo2{};
	allocInfo2.pNext = nullptr;
	allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo2.descriptorPool = descriptorpool;
	allocInfo2.descriptorSetCount  = 1;
	allocInfo2.pSetLayouts = &singletexturesetlayout;

	VK_ASSERT(vkAllocateDescriptorSets(renderer.getdevice()->getlogicaldevice(), &allocInfo2, &dsset), "failed to allocate descriptor sets!");

	std::array<VkDescriptorImageInfo, 1> descriptors{};

	descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptors[0].sampler = texturehandler.gettexture(texture)->sampler;
	descriptors[0].imageView = texturehandler.gettexture(texture)->imageview;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets;

	writeDescriptorSets.push_back(writedescriptorbuffer(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (dsset), 0, 1));
	writeDescriptorSets[0].pImageInfo = &descriptors[0];

	vkUpdateDescriptorSets(renderer.getdevice()->getlogicaldevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

	if (textureset.empty()){
		textureset.reserve(texturehandler.resources.size());
	}
	textureset.push_back(dsset);
}

void DescriptorBuilder::updatesamplerdescriptors2(std::string texture,std::string texture2) {



	VkDescriptorSet dsset;

	VkDescriptorSetAllocateInfo allocInfo2{};
	allocInfo2.pNext = nullptr;
	allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo2.descriptorPool = descriptorpool;
	allocInfo2.descriptorSetCount  = 1;
	allocInfo2.pSetLayouts = &singletexturesetlayout;

	VK_ASSERT(vkAllocateDescriptorSets(renderer.getdevice()->getlogicaldevice(), &allocInfo2, &dsset), "failed to allocate descriptor sets!");

	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.sampler = texturehandler.gettexture(texture)->sampler;
	imageBufferInfo.imageView = texturehandler.gettexture(texture)->imageview;
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkDescriptorImageInfo imageBufferInfo2;
	imageBufferInfo2.sampler = texturehandler.gettexture(texture2)->sampler;
	imageBufferInfo2.imageView = texturehandler.gettexture(texture2)->imageview;
	imageBufferInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet texture1 = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (dsset), 0, 1);
	texture1.pImageInfo = &imageBufferInfo;

	vkUpdateDescriptorSets(renderer.getdevice()->getlogicaldevice(), 1, &texture1, 0, nullptr);

	VkWriteDescriptorSet texture22 = writedescriptorbuffer(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (dsset), 1, 1);
	texture22.pImageInfo = &imageBufferInfo2;

	vkUpdateDescriptorSets(renderer.getdevice()->getlogicaldevice(), 1, &texture22, 0, nullptr);


	if (textureset.empty()){
		textureset.reserve(texturehandler.resources.size());
	}
	textureset.push_back(dsset);
}
