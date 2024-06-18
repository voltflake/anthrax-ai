#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkbuffer.h"
#include "anthraxAI/vktexture.h"
#include "anthraxAI/vkdevices.h"

class DescriptorBuilder {
public:
	void init(DeviceBuilder& deviceh, RenderBuilder& rendererh, DeletionQueue* deletor, TextureBuilder& texture) { device = deviceh; deletorhandler = deletor; renderer = rendererh; texturehandler = texture;};

	void builddescriptors();

	void updatesamplerdescriptors(std::string texture);
	void updateattachmentdescriptors(DeviceBuilder& dev);
	void updatesamplerdescriptors2(std::string texture,std::string texture2);

	size_t paduniformbuffersize(size_t originalsize);

	VkDescriptorSetLayout& getgloballayout() { return globalsetlayout; };
	VkDescriptorSetLayout& getstoragelayout() { return storagesetlayout; };
	VkDescriptorSetLayout& getsamplerlayout() { return singletexturesetlayout; };

	std::vector<VkDescriptorSet>& getmainsamplerdescriptor() { return textureset;};
	VkDescriptorSet& getattachmentdescriptor() { return attachmentset;};

	void cleartextureset();

	UboArray& 						getcamerabuffer() 		{return CameraBuffer;};
	StorageArray& 					getstoragebuffer() 		{return StorageBuffer;};

	std::vector<VkDescriptorSet>& getdescriptorset() { return descriptorsets;};
	std::vector<VkDescriptorSet>& getstorageset() { return storagedescsets;};

	TextureBuilder texturehandler;

private:
	VkDescriptorSetLayout globalsetlayout;
	VkDescriptorSetLayout storagesetlayout;
	VkDescriptorSetLayout singletexturesetlayout;

	std::vector<VkDescriptorSet> textureset;
	VkDescriptorSet attachmentset;
	VkDescriptorPool descriptorpool;
	std::vector<VkDescriptorSet> descriptorsets;
	std::vector<VkDescriptorSet> storagedescsets;

	UboArray 		CameraBuffer;
	StorageArray	StorageBuffer;

	DeletionQueue*	deletorhandler;
	RenderBuilder renderer;
	DeviceBuilder device;
};