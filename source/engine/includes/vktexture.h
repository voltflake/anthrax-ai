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
	float w;
	float h;
};

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