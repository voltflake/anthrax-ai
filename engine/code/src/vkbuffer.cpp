
#include "anthraxAI/vkbuffer.h"

uint32_t BufferBuilder::findmemorytype(VkPhysicalDevice& physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) 
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

void BufferBuilder::allocbuffer(RenderBuilder& renderer, BufferHandler& bufhandler, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateBuffer(renderer.getdevice()->getlogicaldevice(), &bufferInfo, nullptr, &bufhandler.buffer), "failed to create buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderer.getdevice()->getlogicaldevice(), bufhandler.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findmemorytype(renderer.getdevice()->getphysicaldevice(), memRequirements.memoryTypeBits, properties);

    VK_ASSERT(vkAllocateMemory(renderer.getdevice()->getlogicaldevice(), &allocInfo, nullptr, &bufhandler.devicememory), "failed to allocate buffer memory!");
    vkBindBufferMemory(renderer.getdevice()->getlogicaldevice(), bufhandler.buffer, bufhandler.devicememory, 0);
}

void BufferBuilder::copybuffer(RenderBuilder& renderer, VkBuffer& srcbuffer, VkBuffer& dstbuffer, VkDeviceSize size) {
    renderer.submit([=](VkCommandBuffer cmd) {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(cmd, srcbuffer, dstbuffer, 1, &copyRegion);
    });
}

void BufferBuilder::createbuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkBufferUsageFlags flags[2], VkDeviceSize buffersize, const void *datasrc) {

    BufferHandler stagingbuffer;
    allocbuffer(renderer, stagingbuffer, buffersize, flags[0]/*VK_BUFFER_USAGE_TRANSFER_SRC_BIT*/, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* datadst;
    vkMapMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory, 0, buffersize, 0, &datadst);
        memcpy(datadst, datasrc, (size_t)buffersize);
    vkUnmapMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory);

    allocbuffer(renderer, bufferhandler, buffersize, flags[1]/*VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT*/, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copybuffer(renderer, stagingbuffer.buffer, bufferhandler.buffer, buffersize);

    vkDestroyBuffer(renderer.getdevice()->getlogicaldevice(), stagingbuffer.buffer, nullptr);
    vkFreeMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory, nullptr);
}

void BufferBuilder::crearetexturebuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkDeviceSize buffersize, void *pixels) {
   
    allocbuffer(renderer, bufferhandler, buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* datadst;
    vkMapMemory(renderer.getdevice()->getlogicaldevice(), bufferhandler.devicememory, 0, buffersize, 0, &datadst);
        memcpy(datadst, pixels, (size_t)buffersize);
    vkUnmapMemory(renderer.getdevice()->getlogicaldevice(), bufferhandler.devicememory);
}

void BufferBuilder::createbuffer(RenderBuilder& renderer, BufferHandler& bufferhandler, VkDeviceSize buffersize, VkBufferUsageFlagBits usage) {
    
    if (usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        allocbuffer(renderer, bufferhandler, buffersize, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    else {
        allocbuffer(renderer, bufferhandler, buffersize, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
}