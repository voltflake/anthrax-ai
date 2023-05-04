#include "../includes/vktexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void TextureBuilder::loadimages() {
	Texture texture;
	
	createtexture(texture);
	createtextureimageview(texture);
	createsampler(texture);

	loadedtextures["first"] = texture;

	std::cout << "Texture loaded successfully "<< std::endl;

}

void TextureBuilder::createtexture(Texture& texture) {

	BufferBuilder buffer;

	int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("./textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imagesize = texWidth * texHeight * 4;

    ASSERT(!pixels, "failed to load texture image!");

    buffer.crearetexturebuffer(renderer, texturebuffer, imagesize, pixels);
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

	VK_ASSERT(vkCreateImage(devicehandler.getlogicaldevice(), &imageInfo, nullptr, &texture.image), "failed to create image");

	VkMemoryRequirements memrequirements;
	vkGetImageMemoryRequirements(devicehandler.getlogicaldevice(), texture.image, &memrequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memrequirements.size;
	allocInfo.memoryTypeIndex = buffer.findmemorytype(devicehandler.getphysicaldevice(), memrequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VK_ASSERT(vkAllocateMemory(devicehandler.getlogicaldevice(), &allocInfo, nullptr, &texture.memory),"failed to allocate image memory!");

	vkBindImageMemory(devicehandler.getlogicaldevice(), texture.image, texture.memory, 0);
	
	renderer.immediatesubmit([&](VkCommandBuffer cmd) {
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image = texture.image;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		//barrier the image into the transfer-receive layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = { texWidth, texHeight, 1};

		//copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, texturebuffer.buffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//barrier the image into the shader readable layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
	});

    vkDestroyBuffer(devicehandler.getlogicaldevice(), texturebuffer.buffer, nullptr);
    vkFreeMemory(devicehandler.getlogicaldevice(), texturebuffer.devicememory, nullptr);

	deletorhandler.pushfunction([=]() {
		vkDestroyImage(devicehandler.getlogicaldevice(), texture.image, nullptr);
	    vkFreeMemory(devicehandler.getlogicaldevice(), texture.memory, nullptr);
	});


}

void TextureBuilder::createtextureimageview(Texture& texture) {

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

	VK_ASSERT(vkCreateImageView(devicehandler.getlogicaldevice(), &viewInfo, nullptr, &texture.imageview), "failed to create texture image view!");

	deletorhandler.pushfunction([=]() {
			vkDestroyImageView(devicehandler.getlogicaldevice(), texture.imageview, nullptr);
		});
}

void TextureBuilder::createsampler(Texture& texture) {

VkPhysicalDeviceProperties properties{};
vkGetPhysicalDeviceProperties(devicehandler.getphysicaldevice(), &properties);
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
samplerInfo.mipLodBias = 0.0f;
samplerInfo.minLod = 0.0f;
samplerInfo.maxLod = 0.0f;

	VK_ASSERT(vkCreateSampler(devicehandler.getlogicaldevice(), &samplerInfo, nullptr, &texture.sampler), "failed to create sampler!");

	deletorhandler.pushfunction([=]() { 
		vkDestroySampler(devicehandler.getlogicaldevice(), texture.sampler, nullptr);
	});
}
