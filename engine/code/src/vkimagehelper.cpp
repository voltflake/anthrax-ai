#include "anthraxAI/vktexture.h"

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

void ImageHelper::memorybarrier(VkCommandBuffer cmd, VkImage image, VkImageLayout oldlayout, VkImageLayout newlayout) 
{
    VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldlayout;
        barrier.newLayout = newlayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask =  (newlayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags src;
        VkPipelineStageFlags dst;
            src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
// switch (oldlayout) {
//     case VK_IMAGE_LAYOUT_UNDEFINED:
//             barrier.srcAccessMask = 0;
//     barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     break ;
//   case VK_IMAGE_LAYOUT_PREINITIALIZED:
//     barrier.srcAccessMask =
//         VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//         barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//         src = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//     barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//     barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//     barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//     break;
// }

// switch (newlayout) {
//   case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//     barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//     dst = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//   barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//     src = VK_PIPELINE_STAGE_TRANSFER_BIT;

//     barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//      barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//      dst = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     // barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//     // src = 0;
//     break;
//   case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//     barrier.dstAccessMask |=
//         VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//     break;
//   case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//     barrier.srcAccessMask =
//         VK_ACCESS_TRANSFER_WRITE_BIT;
//         src = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//     dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//     break;
//     case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
//                 barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
// 	        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//             src = VK_PIPELINE_STAGE_TRANSFER_BIT;
//             dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     break;
// }

    src = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    dst= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        // if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        //     barrier.srcAccessMask = 0;
        //     barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        //     src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // } else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        //     barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //     barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        // } else if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_GENERAL) {
        //     barrier.srcAccessMask = 0;
        //     barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // }
        // else if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        //     barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //     barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // }
        // else if (oldlayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        //     barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  	 	//     barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // }
        // else if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        //     barrier.srcAccessMask = 0;
        //     barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // }
        // else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        //     barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	    //     barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        //     src = VK_PIPELINE_STAGE_TRANSFER_BIT;
        //     dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        // }
        // else {
        //     ASSERT(false, "transition image layout: unsupported layout transition!");
        // }

        vkCmdPipelineBarrier(
            cmd,
            src, dst,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
}

VkImageSubresourceRange ImageHelper::subresrange(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subImage {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}

void ImageHelper::memorybarrierall(VkCommandBuffer cmd, VkImage image, VkImageLayout oldlayout, VkImageLayout newlayout) 
{
    VkImageMemoryBarrier2 imageBarrier {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = oldlayout;
    imageBarrier.newLayout = newlayout;

    VkImageAspectFlags aspectMask = (newlayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = subresrange(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
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

void ImageHelper::copytoimage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
{
   
    VkOffset3D blitSize;
	blitSize.x = dstSize.width;
	blitSize.y = dstSize.height;
	blitSize.z = 1;
        VkOffset3D blitSizer;
	blitSizer.x = srcSize.width;
	blitSizer.y = srcSize.height;
	blitSizer.z = 1;
	VkImageBlit imageBlitRegion{};
	imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.srcSubresource.layerCount = 1;
	imageBlitRegion.srcOffsets[1] = blitSizer;
	imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.dstSubresource.layerCount = 1;
	imageBlitRegion.dstOffsets[1] = blitSize;

	vkCmdBlitImage(
		cmd,
	 source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageBlitRegion,
		VK_FILTER_NEAREST);
	// VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	// blitRegion.srcOffsets[1].x = srcSize.width;
	// blitRegion.srcOffsets[1].y = srcSize.height;
	// blitRegion.srcOffsets[1].z = 1;

	// blitRegion.dstOffsets[1].x = dstSize.width;
	// blitRegion.dstOffsets[1].y = dstSize.height;
	// blitRegion.dstOffsets[1].z = 1;

	// blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// blitRegion.srcSubresource.baseArrayLayer = 0;
	// blitRegion.srcSubresource.layerCount = 1;
	// blitRegion.srcSubresource.mipLevel = 0;

	// blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// blitRegion.dstSubresource.baseArrayLayer = 0;
	// blitRegion.dstSubresource.layerCount = 1;
	// blitRegion.dstSubresource.mipLevel = 0;

	// VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	// blitInfo.dstImage = destination;
	// blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	// blitInfo.srcImage = source;
	// blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	// blitInfo.filter = VK_FILTER_LINEAR;
	// blitInfo.regionCount = 1;
	// blitInfo.pRegions = &blitRegion;

	// vkCmdBlitImage2(cmd, &blitInfo);
}
