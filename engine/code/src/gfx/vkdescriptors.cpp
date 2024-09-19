#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkrenderer.h"

size_t Gfx::DescriptorsBase::PadUniformBufferSize(size_t originalsize)
{
	size_t minUboAlignment = Gfx::Device::GetInstance()->MinUniformBufferOffsetAlignment;
	size_t alignedSize = originalsize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}

VkDescriptorSetLayoutBinding DescriptorLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stageFlags;
	return setbind;
}

void Gfx::DescriptorsBase::Init()
{
	Allocator = new Gfx::DescriptorAllocator{};
	LayoutCache = new Gfx::DescriptorLayoutCache{};

	VkDescriptorSetLayoutBinding texturebind = DescriptorLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.bindingCount = 1;
	setinfo.flags = 0;
	setinfo.pNext = nullptr;
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pBindings = &texturebind;

	TextureSetLayout = LayoutCache->CreateDescriptorLayout(&setinfo);

	const size_t camerabuffersize = MAX_FRAMES * PadUniformBufferSize(sizeof(CameraData));
	for (int i = 0; i < MAX_FRAMES; i++) {		
		Gfx::Renderer::GetInstance()->Frames[i].DynamicDescAllocator = new Gfx::DescriptorAllocator{};

	// 	// auto dynamicDataBuffer = create_buffer(1000000, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
	// 	// _frames[i].dynamicData.init(_allocator, dynamicDataBuffer, _gpuProperties.limits.minUniformBufferOffsetAlignment); 

	// 	// //20 megabyte of debug output
	// 	// _frames[i].debugOutputBuffer = create_buffer(200000000, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU);
	}
}

bool Gfx::DescriptorAllocator::Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
{
	if (Pool == VK_NULL_HANDLE) {
		Pool = GrabPool();
		UsedPools.push_back(Pool);
	}

	VkDescriptorSetAllocateInfo allocinfo = {};
	allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocinfo.pNext = nullptr;
	allocinfo.pSetLayouts = &layout;
	allocinfo.descriptorPool = Pool;
	allocinfo.descriptorSetCount = 1;

	VkResult result = vkAllocateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), &allocinfo, set);
	bool needreallocate = false;
	switch (result) {
		case VK_SUCCESS:
			return true;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			needreallocate = true;
			break;
		default:
			return false;
	}

	if (needreallocate) {
		Pool = GrabPool();
		UsedPools.push_back(Pool);
		result = vkAllocateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), &allocinfo, set);
	}
	return result == VK_SUCCESS;
}

void Gfx::DescriptorAllocator::ResetPools()
{
	for (VkDescriptorPool pool : UsedPools) {
		vkResetDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), pool, 0);
		FreePools.push_back(pool);
	}
	UsedPools.clear();
	Pool = VK_NULL_HANDLE;
}

void Gfx::DescriptorAllocator::CleanUp()
{
	for (VkDescriptorPool pool : FreePools) {
		vkDestroyDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), pool, nullptr);
	}
	for (VkDescriptorPool pool : UsedPools) {
		vkDestroyDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), pool, nullptr);
	}
}

VkDescriptorPool Gfx::DescriptorAllocator::GrabPool()
{
	// if we can reuse pool
	if (FreePools.size() > 0) {
		VkDescriptorPool pool = FreePools.back();
		FreePools.pop_back();
		return pool;
	}
	else {
		return CreatePool(1000, DescriptorPoolSizes);
	}
}

VkDescriptorPool Gfx::DescriptorAllocator::CreatePool(int count, const DescriptorAllocator::PoolSizes& poolsizes)
{
	std::vector<VkDescriptorPoolSize> sizes;
	sizes.reserve(poolsizes.Sizes.size());
	for (auto poolsz : poolsizes.Sizes) {
		sizes.push_back({ poolsz.first, uint32_t(poolsz.second * count) });
	}
	VkDescriptorPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.maxSets = count;
	poolinfo.poolSizeCount = (uint32_t)sizes.size();
	poolinfo.pPoolSizes = sizes.data();

	VkDescriptorPool pool;
	vkCreateDescriptorPool(Gfx::Device::GetInstance()->GetDevice(), &poolinfo, nullptr, &pool);
	return pool;
}

void Gfx::DescriptorLayoutCache::CleanUp(){
	for (auto pair : LayoutCache){
		vkDestroyDescriptorSetLayout(Gfx::Device::GetInstance()->GetDevice(), pair.second, nullptr);
	}
}

VkDescriptorSetLayout Gfx::DescriptorLayoutCache::CreateDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info)
{
	DescriptorLayoutInfo layoutinfo;
	layoutinfo.Bindings.reserve(info->bindingCount);
	bool issorted = true;
	int lastbinding = -1;

	for (int i = 0; i < info->bindingCount; i++) {
		layoutinfo.Bindings.push_back(info->pBindings[i]);
		if (info->pBindings[i].binding > lastbinding) {
			lastbinding = info->pBindings[i].binding;
		}
		else {
			issorted = false;
		}
	}

	if (!issorted){
		std::sort(layoutinfo.Bindings.begin(), layoutinfo.Bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b ) {
			return a.binding < b.binding;
		});
	}

	VkDescriptorSetLayout layout;
	auto it = LayoutCache.find(layoutinfo);
	if (it != LayoutCache.end()){
		return (*it).second;
	}
	else {
		vkCreateDescriptorSetLayout(Gfx::Device::GetInstance()->GetDevice(), info, nullptr, &layout);
		LayoutCache[layoutinfo] = layout;
		return layout;
	}
	return layout;
}

bool Gfx::DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
{
	if (other.Bindings.size() != Bindings.size()){
		return false;
	}
	else {
		for (int i = 0; i < Bindings.size(); i++) {
			if (other.Bindings[i].binding != Bindings[i].binding){
				return false;
			}
			if (other.Bindings[i].descriptorType != Bindings[i].descriptorType){
				return false;
			}
			if (other.Bindings[i].descriptorCount != Bindings[i].descriptorCount){
				return false;
			}
			if (other.Bindings[i].stageFlags != Bindings[i].stageFlags){
				return false;
			}
		}
		return true;
	}
}

size_t Gfx::DescriptorLayoutCache::DescriptorLayoutInfo::Hash() const
{
	using std::size_t;
	using std::hash;

	size_t result = hash<size_t>()(Bindings.size());
	for (const VkDescriptorSetLayoutBinding& b : Bindings)
	{
		//pack the binding data into a single int64
		size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;
		//shuffle the packed binding data and xor it with the main hash
		result ^= hash<size_t>()(binding_hash);
	}
	return result;
}

Gfx::Descriptors Gfx::Descriptors::Begin(DescriptorLayoutCache* layoutcache, DescriptorAllocator* allocator)
{
	Descriptors desc;
	desc.Cache = layoutcache;
	desc.Allocator = allocator;
	return desc;
}

Gfx::Descriptors& Gfx::Descriptors::BindBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferinfo, VkDescriptorType type, VkShaderStageFlags stageflags)
{
	VkDescriptorSetLayoutBinding newbinding{};
	newbinding.descriptorCount = 1;
	newbinding.descriptorType = type;
	newbinding.pImmutableSamplers = nullptr;
	newbinding.stageFlags = stageflags;
	newbinding.binding = binding;
	Bindings.push_back(newbinding);

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = bufferinfo;
	write.dstBinding = binding;
	Writes.push_back(write);
	return *this;
}

Gfx::Descriptors& Gfx::Descriptors::BindImage(uint32_t binding, VkDescriptorImageInfo* imageinfo, VkDescriptorType type, VkShaderStageFlags stageflags)
{
	VkDescriptorSetLayoutBinding bind{};
	bind.descriptorCount = 1;
	bind.descriptorType = type;
	bind.pImmutableSamplers = nullptr;
	bind.stageFlags = stageflags;
	bind.binding = binding;
	Bindings.push_back(bind);

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pImageInfo = imageinfo;
	write.dstBinding = binding;
	Writes.push_back(write);
	return *this;
}

bool Gfx::Descriptors::Build(VkDescriptorSet& set)
{
	VkDescriptorSetLayout layout;
	return Build(set, layout);
}

bool Gfx::Descriptors::Build(VkDescriptorSet& set, VkDescriptorSetLayout& layout){
	VkDescriptorSetLayoutCreateInfo layoutinfo{};
	layoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutinfo.pNext = nullptr;
	layoutinfo.pBindings = Bindings.data();
	layoutinfo.bindingCount = Bindings.size();

	layout = Cache->CreateDescriptorLayout(&layoutinfo);

	bool success = Allocator->Allocate(&set, layout);
	if (!success) { 
		return false;
	};

	for (VkWriteDescriptorSet& w : Writes) {
		w.dstSet = set;
	}
	vkUpdateDescriptorSets(Gfx::Device::GetInstance()->GetDevice(), Writes.size(), Writes.data(), 0, nullptr);
	return true;
}

void Gfx::DescriptorsBase::CleanUp()
{
	Allocator->CleanUp();
	LayoutCache->CleanUp();
}