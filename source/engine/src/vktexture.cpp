#include "../includes/vktexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void TextureBuilder::loadimages() {
	
	for (auto& list : resources) {
        if (list.second.texturepath == "") {
            continue;
        }
		std::string path = "./textures/";
		path += list.second.texturepath;

		std::cout << "textures|"<< list.second.pos.x << " ----- " << list.second.pos.y << "\n";

		createtexture(path);
		createtextureimageview();
		createsampler();

		loadedtextures[list.second.texturepath] = texture;
	}

}

void TextureBuilder::cleartextures() {
    for (auto& list : loadedtextures) {
        vkDestroySampler(devicehandler.getlogicaldevice(), list.second.sampler, nullptr);
		vkDestroyImageView(devicehandler.getlogicaldevice(), list.second.imageview, nullptr);
        vkDestroyImage(devicehandler.getlogicaldevice(), list.second.image, nullptr);
	    vkFreeMemory(devicehandler.getlogicaldevice(), list.second.memory, nullptr);
    }
    loadedtextures.clear();
}

void TextureBuilder::createtexture(const std::string& texturepath) {

	BufferBuilder buffer;

	int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(texturepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imagesize = texWidth * texHeight * 4;

    texture.w = (float)texWidth;
    texture.h = (float)texHeight;

    std::cout << texturepath << '\n';

    ASSERT(!pixels, "failed to load texture image!");

    BufferHandler stagingbuffer;
    buffer.allocbuffer(renderer, stagingbuffer, imagesize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* datadst;
    vkMapMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory, 0, imagesize, 0, &datadst);
        memcpy(datadst, pixels, (size_t)imagesize);
    vkUnmapMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory);
   
    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VK_ASSERT(vkCreateImage(renderer.getdevice()->getlogicaldevice(), &imageInfo, nullptr, &texture.image), "failed to create image");

	VkMemoryRequirements memrequirements;
	vkGetImageMemoryRequirements(renderer.getdevice()->getlogicaldevice(), texture.image, &memrequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memrequirements.size;
	allocInfo.memoryTypeIndex = buffer.findmemorytype(devicehandler.getphysicaldevice(), memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_ASSERT(vkAllocateMemory(renderer.getdevice()->getlogicaldevice(), &allocInfo, nullptr, &texture.memory),"failed to allocate image memory!");

	vkBindImageMemory(renderer.getdevice()->getlogicaldevice(), texture.image, texture.memory, 0);

	renderer.immediatesubmit([&](VkCommandBuffer cmd) {
		 VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texture.image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        vkCmdPipelineBarrier(
            cmd,
             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

	});

		renderer.immediatesubmit([&](VkCommandBuffer cmd){

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
            static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
            1
        };

        vkCmdCopyBufferToImage(cmd, stagingbuffer.buffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	});
	renderer.immediatesubmit([&](VkCommandBuffer cmd) {

		 VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texture.image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
	});

    vkDestroyBuffer(renderer.getdevice()->getlogicaldevice(), stagingbuffer.buffer, nullptr);
    vkFreeMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory, nullptr);

	// deletorhandler->pushfunction([=]() {
	// 	vkDestroyImage(devicehandler.getlogicaldevice(), texture.image, nullptr);
	//     vkFreeMemory(devicehandler.getlogicaldevice(), texture.memory, nullptr);
	// });


}

void TextureBuilder::createtextureimageview() {

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = texture.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VK_ASSERT(vkCreateImageView(renderer.getdevice()->getlogicaldevice(), &viewInfo, nullptr, &texture.imageview), "failed to create texture image view!");

	// deletorhandler->pushfunction([=]() {
	// 		vkDestroyImageView(devicehandler.getlogicaldevice(), texture.imageview, nullptr);
	// 	});
}

void TextureBuilder::createsampler() {

//VkPhysicalDeviceProperties properties{};
//vkGetPhysicalDeviceProperties(devicehandler.getphysicaldevice(), &properties);
	VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerinfo.pNext = nullptr;

	samplerinfo.magFilter = VK_FILTER_NEAREST;
	samplerinfo.minFilter = VK_FILTER_NEAREST;
	samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VK_ASSERT(vkCreateSampler(renderer.getdevice()->getlogicaldevice(), &samplerinfo, nullptr, &texture.sampler), "failed to create sampler!");

	// deletorhandler->pushfunction([=]() { 
	// 	vkDestroySampler(devicehandler.getlogicaldevice(), texture.sampler, nullptr);
	// });
}
