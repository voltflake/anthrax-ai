#pragma  once
#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkbuffer.h"

namespace ImageHelper
{
	void createimage(VkImageCreateInfo imginfo, DeviceBuilder& device, AllocatedImage* img);
	void createimageview(VkImageViewCreateInfo viewinfo, DeviceBuilder& device, AllocatedImage* img);
	void memorybarrier(VkCommandBuffer cmd, VkImage image, VkImageLayout oldlayout, VkImageLayout newlayout);
	void memorybarrierall(VkCommandBuffer cmd, VkImage image, VkImageLayout oldlayout, VkImageLayout newlayout); 
	void copy(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void copytoimage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);

	VkImageSubresourceRange subresrange(VkImageAspectFlags aspectMask);
	VkImageCreateInfo imagecreateinfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo imageviewcreateinfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
}

class TextureBuilder {
public:
	void init(RenderBuilder& renderb, DeviceBuilder& device, DeletionQueue* deletor, ResourcesMap& res) { renderer = renderb; devicehandler = device; deletorhandler = deletor; resources = res;};
	void createtexture(const std::string& texturepath);
	void createtextureimageview();
	void createsampler();

	void buildimagesview();
	void loadimages();

	void clearimages() { cleartextures(); resources.clear();};

	void cleartextures();

	void createmainimage(DeviceBuilder& device);
	void createdepthbuffer(DeviceBuilder& device);

	void clearattachments(DeviceBuilder& device);

	Texture* gettexture(std::string name) { auto it = loadedtextures.find(name);
												if (it == loadedtextures.end()) {
													return nullptr;
												}
												else {
													return &(*it).second;
												}
											}
	ResourcesMap resources;

private:
	RenderBuilder 	renderer;
	DeviceBuilder	devicehandler;
	DeletionQueue* 	deletorhandler;

	Texture texture;

	BufferHandler 	texturebuffer;
	std::unordered_map<std::string, Texture> loadedtextures;
};