#pragma  once
#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkbuffer.h"

namespace ImageHelper
{
	void createimage(VkImageCreateInfo imginfo, DeviceBuilder& device, AllocatedImage* img);
	void createimageview(VkImageViewCreateInfo viewinfo, DeviceBuilder& device, AllocatedImage* img);
	void memorybarrier(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldlayout, VkImageLayout newlayout); 
	void copy(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkImageCreateInfo imagecreateinfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo imageviewcreateinfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
}

class TextureBuilder {
public:
	void init(RenderBuilder& renderb, DeviceBuilder& device, DeletionQueue* deletor, std::unordered_map<int, Data>& res) { renderer = renderb; devicehandler = device; deletorhandler = deletor; resources = res;};
	void createtexture(const std::string& texturepath);
	void createtextureimageview();
	void createsampler();

	void buildimagesview();
	void loadimages();

	void clearimages() { cleartextures(); resources.clear();};

	void cleartextures();

	void createdepthbuffer(DeviceBuilder& device);

	Texture* gettexture(std::string name) { auto it = loadedtextures.find(name);
												if (it == loadedtextures.end()) {
													return nullptr;
												}
												else {
													return &(*it).second;
												}
											}
	std::unordered_map<int, Data> resources;


private:
	RenderBuilder 	renderer;
	DeviceBuilder	devicehandler;
	DeletionQueue* 	deletorhandler;

	Texture texture;

	BufferHandler 	texturebuffer;
	std::unordered_map<std::string, Texture> loadedtextures;
};