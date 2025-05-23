#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkdevice.h"

size_t Gfx::DescriptorsBase::PadUniformBufferSize(size_t originalsize)
{
	size_t minalignment = Gfx::Device::GetInstance()->MinUniformBufferOffsetAlignment;
	size_t alignedsize = originalsize;
	if (minalignment > 0) {
		alignedsize = (alignedsize + minalignment - 1) & ~(minalignment - 1);
	}
	return alignedsize;
}

VkDescriptorSetLayoutBinding DescriptorLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 5000;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stageFlags;
	return setbind;
}

uint32_t Gfx::DescriptorsBase::UpdateTexture(VkImageView imageview, VkSampler sampler)
{
	VkDescriptorImageInfo imageinfo{};
	imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageinfo.imageView = imageview;
	imageinfo.sampler = sampler;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.dstBinding = TextureBinding;
	write.dstSet = BindlessDescriptor;
	write.descriptorCount = 1;
	write.dstArrayElement = TextureHandle;
	write.pImageInfo = &imageinfo;

	vkUpdateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), 1, &write, 0, nullptr);
	TextureHandle++;

	return TextureHandle - 1;
}

uint32_t Gfx::DescriptorsBase::UpdateBuffer(VkBuffer buffer, VkBufferUsageFlagBits usage)
{
	VkWriteDescriptorSet writes{};
	VkDescriptorBufferInfo bufferinfo{};
	bufferinfo.buffer = buffer;
	bufferinfo.offset = 0;
	bufferinfo.range = VK_WHOLE_SIZE;
	writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes.dstSet = BindlessDescriptor;
	writes.descriptorCount = 1;
	writes.dstArrayElement = BufferHandle;
	writes.pBufferInfo = &bufferinfo;

	if ((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
		writes.dstBinding = UniformBinding;
		writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}
	if ((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
		writes.dstBinding = StorageBinding;
		writes.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	}

	vkUpdateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), 1, &writes, 0, nullptr);
	BufferHandle++;

	return BufferHandle - 1;
}

void Gfx::DescriptorsBase::AllocateDataBuffers()
{
	const size_t cambuffersize = (sizeof(CameraData));
	BufferHelper::CreateBuffer(CameraBuffer, cambuffersize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	VkDebugUtilsObjectNameInfoEXT info;
	info.pNext = nullptr;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectHandle = reinterpret_cast<uint64_t>(CameraBuffer.DeviceMemory);
	info.objectType = VK_OBJECT_TYPE_DEVICE_MEMORY;;
	info.pObjectName = "data buffer";
	Gfx::Vulkan::GetInstance()->SetDebugName(info);

	Core::Deletor::GetInstance()->Push(Core::Deletor::Type::NONE, [=, this]() {
		vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), CameraBuffer.Buffer, nullptr);
    	vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), CameraBuffer.DeviceMemory, nullptr);
	});
}
void Gfx::DescriptorsBase::AllocateStorageBuffers()
{
	size_t buffersize = (sizeof(StorageData));
	BufferHelper::CreateBuffer(StorageBuffer, buffersize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	VkDebugUtilsObjectNameInfoEXT info;
	info.pNext = nullptr;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectHandle = reinterpret_cast<uint64_t>(StorageBuffer.DeviceMemory);
	info.objectType = VK_OBJECT_TYPE_DEVICE_MEMORY;;
	info.pObjectName = "storage buffer";
	Gfx::Vulkan::GetInstance()->SetDebugName(info);

	Core::Deletor::GetInstance()->Push(Core::Deletor::Type::NONE, [=, this]() {
		vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), StorageBuffer.Buffer, nullptr);
    	vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), StorageBuffer.DeviceMemory, nullptr);
	});

    buffersize = sizeof(InstanceData) * MAX_INSTANCES ;
	BufferHelper::CreateBuffer(InstanceBuffer, buffersize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	info.pNext = nullptr;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	info.objectHandle = reinterpret_cast<uint64_t>(InstanceBuffer.DeviceMemory);
	info.objectType = VK_OBJECT_TYPE_DEVICE_MEMORY;;
	info.pObjectName = "INSTANCE buffer";
	Gfx::Vulkan::GetInstance()->SetDebugName(info);

    Core::Deletor::GetInstance()->Push(Core::Deletor::Type::NONE, [=, this]() {
		vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), InstanceBuffer.Buffer, nullptr);
    	vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), InstanceBuffer.DeviceMemory, nullptr);
	});
}

void Gfx::DescriptorsBase::CleanAll()
{
	TextureHandle = 0;
    BufferHandle = 0;

	vkDestroyDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), Pool, nullptr);

	vkDestroyDescriptorSetLayout(Gfx::Device::GetInstance()->GetDevice(), BindlessLayout, nullptr);
}

void Gfx::DescriptorsBase::CleanBindless()
{
	vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), BindlessBuffer.Buffer, nullptr);
    vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), BindlessBuffer.DeviceMemory, nullptr);
}

void Gfx::DescriptorsBase::Init()
{
// bindless descriptor setup

	VkDescriptorSetLayoutBinding bindings[MAX_BINDING];
	VkShaderStageFlags stageflags[MAX_BINDING] = {
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	VkDescriptorBindingFlags flags[MAX_BINDING];
	VkDescriptorType types[MAX_BINDING] = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	};

// descriptor pool

	VkDescriptorPoolSize sizes[MAX_BINDING] = {
		{ types[0], 5000 }, { types[1], 5000 }, { types[2], 5000 }
	};

	VkDescriptorPoolCreateInfo poolinfo{};
	poolinfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.maxSets = 5000;
	poolinfo.poolSizeCount = MAX_BINDING;
	poolinfo.pPoolSizes = sizes;
	vkCreateDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &Pool);

// bindless layout

	for (int i = 0; i < MAX_BINDING; i++) {
		bindings[i] = DescriptorLayoutBinding(types[i], stageflags[i], i);
		flags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingflags{};
	bindingflags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingflags.pNext = nullptr;
	bindingflags.pBindingFlags = flags;
	bindingflags.bindingCount = MAX_BINDING;

	VkDescriptorSetLayoutCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createinfo.bindingCount = MAX_BINDING;
	createinfo.pBindings = bindings;
	createinfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	createinfo.pNext = &bindingflags;

	vkCreateDescriptorSetLayout(Gfx::Device::GetInstance()->GetDevice(), &createinfo, nullptr, &BindlessLayout);

// bindless descriptor

	VkDescriptorSetAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocinfo.pNext = nullptr;
	allocinfo.descriptorPool = Pool;
	allocinfo.pSetLayouts = &BindlessLayout;
	allocinfo.descriptorSetCount = 1;
	vkAllocateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), &allocinfo, &BindlessDescriptor);
}

void Gfx::DescriptorsBase::AllocateBuffers()
{
    AllocateDataBuffers();
    AllocateStorageBuffers();
}
