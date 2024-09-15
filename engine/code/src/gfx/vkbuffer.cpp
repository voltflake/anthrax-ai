#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/bufferhelper.h"

void BufferHelper::CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
{

}
void BufferHelper::AllocateBuffer(Buffer& bufhandler, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{

}
void BufferHelper::CreateBuffer(Buffer& bufferhandler, VkBufferUsageFlags flags[2], VkDeviceSize buffersize, const void *datasrc)
{

}
void BufferHelper::CreateBuffer(Buffer& bufferhandler, VkDeviceSize buffersize, VkBufferUsageFlagBits usage)
{

}
void BufferHelper::CreateTextureBuffer(Buffer& bufferhandler, VkDeviceSize buffersize, void *pixels)
{
}
uint32_t BufferHelper::FindMemoryType(VkPhysicalDevice physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicaldevice, &memProperties);
    uint32_t i = 0;
    for (; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    ASSERT(i != 0, "failed to find suitable memory type!");
    return 0;
}

void Gfx::Renderer::AllocateRTMemory(RenderTarget* rt)
{
    VkMemoryRequirements memrequirements;
	vkGetImageMemoryRequirements(Gfx::Device::GetInstance()->GetDevice(), rt->Image, &memrequirements);
    VkMemoryAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocinfo.allocationSize = memrequirements.size;
	allocinfo.memoryTypeIndex = BufferHelper::FindMemoryType(Gfx::Device::GetInstance()->GetPhysicalDevice(), memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_ASSERT(vkAllocateMemory(Gfx::Device::GetInstance()->GetDevice(), &allocinfo, nullptr, &rt->Memory),"failed to allocate image memory!");
	vkBindImageMemory(Gfx::Device::GetInstance()->GetDevice(), rt->Image, rt->Memory, 0);
}