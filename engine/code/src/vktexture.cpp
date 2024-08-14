#include "anthraxAI/vktexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void TextureBuilder::clearattachments(DeviceBuilder& device)
{
	if (device.depthimage.texture) {
		vkDestroyImageView(device.getlogicaldevice(), device.depthimage.texture->imageview, nullptr);
		vkDestroyImage(device.getlogicaldevice(),device.depthimage.texture->image, nullptr);
		vkFreeMemory(device.getlogicaldevice(),device.depthimage.texture->memory, nullptr);
		delete device.depthimage.texture;
	}

	if (device.mainrendertarget.texture) {
		vkDestroyImageView(device.getlogicaldevice(), device.mainrendertarget.texture->imageview, nullptr);
		vkDestroyImage(device.getlogicaldevice(), device.mainrendertarget.texture->image, nullptr);
		vkFreeMemory(device.getlogicaldevice(), device.mainrendertarget.texture->memory, nullptr);
		delete device.mainrendertarget.texture;
	}
}

void TextureBuilder::createdepthbuffer(DeviceBuilder& device)
{
	if (device.depthimage.texture) {
		vkDestroyImageView(device.getlogicaldevice(), device.depthimage.texture->imageview, nullptr);
		vkDestroyImage(device.getlogicaldevice(),device.depthimage.texture->image, nullptr);
		vkFreeMemory(device.getlogicaldevice(),device.depthimage.texture->memory, nullptr);
		delete device.depthimage.texture;
	}
    VkFormat depthformat = VK_FORMAT_D32_SFLOAT;
	device.depthimage.texture = new Texture;
	device.depthimage.texture->format = depthformat;
    ImageHelper::createimage(
		ImageHelper::imagecreateinfo(depthformat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, { static_cast<uint32_t>(device.getswapchainextent().width), static_cast<uint32_t>(device.getswapchainextent().height), 1 }),
		device, &device.depthimage);
	ImageHelper::createimageview(ImageHelper::imageviewcreateinfo(depthformat, device.depthimage.texture->image, VK_IMAGE_ASPECT_DEPTH_BIT),
	device, &device.depthimage);
	std::cout << "created depth image\n";

}

void TextureBuilder::createmainimage(DeviceBuilder& device)
{
	if (device.mainrendertarget.texture) {
		vkDestroyImageView(device.getlogicaldevice(), device.mainrendertarget.texture->imageview, nullptr);
		vkDestroyImage(device.getlogicaldevice(), device.mainrendertarget.texture->image, nullptr);
		vkFreeMemory(device.getlogicaldevice(), device.mainrendertarget.texture->memory, nullptr);
		delete device.mainrendertarget.texture;
	}
	device.mainrendertarget.texture = new Texture;
	device.mainrendertarget.texture->format = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkImageUsageFlags imgusage{};
	imgusage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imgusage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imgusage |= VK_IMAGE_USAGE_STORAGE_BIT;
	imgusage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imgusage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    ImageHelper::createimage(
        ImageHelper::imagecreateinfo(VK_FORMAT_R16G16B16A16_SFLOAT, imgusage, { static_cast<uint32_t>(device.getswapchainextent().width), static_cast<uint32_t>(device.getswapchainextent().height), 1 }),
        device, &device.mainrendertarget);
	ImageHelper::createimageview(ImageHelper::imageviewcreateinfo(VK_FORMAT_R16G16B16A16_SFLOAT, device.mainrendertarget.texture->image, VK_IMAGE_ASPECT_COLOR_BIT),
	device, &device.mainrendertarget);
	std::cout << "created main image\n";
}


void TextureBuilder::loadimages() {

	for (auto& list : resources) {
		auto it = loadedtextures.find(list.second.texturepath);
        if (list.second.texturepath.empty() || it != loadedtextures.end()) {
            continue;
        }
		std::string path = "./textures/";
		if (list.second.material) {
			path = "";
		}
		path += list.second.texturepath;

		std::cout << "textures|"<< list.second.pos.x << " ----- " << list.second.pos.y << "\n";

		createtexture(path);
		createsampler();

		loadedtextures[list.second.texturepath] = texture;
	}

	std::string path = "./textures/";
	path += "dummy.png";
	auto it = loadedtextures.find("dummy.png");
	if (it != loadedtextures.end()) {
		return ;
	}
	createtexture(path);
	createsampler();

	loadedtextures["dummy.png"] = texture;
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

    AllocatedImage imgtmp = { &texture };
    ImageHelper::createimage(
        ImageHelper::imagecreateinfo(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 }),
        devicehandler, &imgtmp);
	renderer.submit([&](VkCommandBuffer cmd) {
        ImageHelper::memorybarrier(cmd, texture.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	});
	renderer.submit([&](VkCommandBuffer cmd){
        ImageHelper::copy(cmd, stagingbuffer.buffer, texture.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	});
	renderer.submit([&](VkCommandBuffer cmd) {
		ImageHelper::memorybarrier(cmd, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	});
    vkDestroyBuffer(renderer.getdevice()->getlogicaldevice(), stagingbuffer.buffer, nullptr);
    vkFreeMemory(renderer.getdevice()->getlogicaldevice(), stagingbuffer.devicememory, nullptr);

	ImageHelper::createimageview(ImageHelper::imageviewcreateinfo(VK_FORMAT_R8G8B8A8_SRGB, texture.image, VK_IMAGE_ASPECT_COLOR_BIT),
	devicehandler, &imgtmp);
}

void TextureBuilder::createsampler() {

	VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerinfo.pNext = nullptr;

	samplerinfo.magFilter = VK_FILTER_NEAREST;
	samplerinfo.minFilter = VK_FILTER_NEAREST;
	samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VK_ASSERT(vkCreateSampler(renderer.getdevice()->getlogicaldevice(), &samplerinfo, nullptr, &texture.sampler), "failed to create sampler!");
}
