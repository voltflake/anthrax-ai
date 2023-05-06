#pragma  once
#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkbuffer.h"


struct Texture {
	VkImage image;
	VkImageView imageview;
	VkDeviceMemory memory;

	VkSampler sampler;
};

class TextureBuilder {
public:
	void init(RenderBuilder& renderb, DeviceBuilder& device, DeletionQueue& deletor) { renderer = renderb; devicehandler = device; deletorhandler = deletor;};
	void createtexture();
	void createtextureimageview();
	void createsampler();


	void buildimagesview();
	void loadimages();


	Texture* gettexture(std::string name) { auto it = loadedtextures.find(name);
												if (it == loadedtextures.end()) {
													return nullptr;
												}
												else {
													return &(*it).second;
												}
											}


private:
	RenderBuilder 	renderer;
	DeviceBuilder	devicehandler;
	DeletionQueue 	deletorhandler;

	Texture texture;

	BufferHandler 	texturebuffer;
	std::unordered_map<std::string, Texture> loadedtextures;

};