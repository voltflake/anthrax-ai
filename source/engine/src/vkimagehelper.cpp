#include "../includes/vktexture.h"

VkImageCreateInfo ImageHelper::imagecreateinfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
    VkImageCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo ImageHelper::imageviewcreateinfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspectFlags;

	return info;
}

void ImageHelper::createimage(VkImageCreateInfo imginfo, DeviceBuilder& device, AllocatedImage* img)
{
	VK_ASSERT(vkCreateImage(device.getlogicaldevice(), &imginfo, nullptr, &img->texture->image), "failed to create image");

	VkMemoryRequirements memrequirements;
	vkGetImageMemoryRequirements(device.getlogicaldevice(), img->texture->image, &memrequirements);

    BufferBuilder buffer;
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memrequirements.size;
	allocInfo.memoryTypeIndex = buffer.findmemorytype(device.getphysicaldevice(), memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_ASSERT(vkAllocateMemory(device.getlogicaldevice(), &allocInfo, nullptr, &img->texture->memory),"failed to allocate image memory!");

	vkBindImageMemory(device.getlogicaldevice(), img->texture->image, img->texture->memory, 0);
}

void ImageHelper::createimageview(VkImageViewCreateInfo viewinfo, DeviceBuilder& device, AllocatedImage* img) {
	VK_ASSERT(vkCreateImageView(device.getlogicaldevice(), &viewinfo, nullptr, &img->texture->imageview), "failed to create texture image view!");
}

void ImageHelper::memorybarrier(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldlayout, VkImageLayout newlayout) 
{
    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldlayout;
        barrier.newLayout = newlayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags src;
        VkPipelineStageFlags dst;

        if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            src = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            ASSERT(false, "transition image layout: unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            cmd,
            src, dst,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
}

void ImageHelper::copy(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        static_cast<uint32_t>(width), static_cast<uint32_t>(height),
        1
    };

    vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}