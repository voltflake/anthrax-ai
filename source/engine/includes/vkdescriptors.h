#pragma once

#include "vkdefines.h"
#include "vkrenderer.h"
#include "vkbuffer.h"
#include "vktexture.h"

class DescriptorBuilder {
public:
	void init(RenderBuilder& rendererh, DeletionQueue& deletor, TextureBuilder& texture) { deletorhandler = deletor; renderer = rendererh; texturehandler = texture;};

	void builddescriptors();

	void updatesamplerdescriptors();

	size_t paduniformbuffersize(size_t originalsize);

	VkDescriptorSetLayout& getgloballayout() { return globalsetlayout; };
	VkDescriptorSetLayout& getsamplerlayout() { return singletexturesetlayout; };

	VkDescriptorSet& getsamplerdescriptor() { return textureset;};


	UboArray& 						getcamerabuffer() 		{return CameraBuffer;};

	std::vector<VkDescriptorSet>& getdescriptorset() { return descriptorsets;};

private:
	VkDescriptorSetLayout globalsetlayout;

	VkDescriptorSetLayout singletexturesetlayout;
	VkDescriptorSet textureset{VK_NULL_HANDLE};

	VkDescriptorPool descriptorpool;
	std::vector<VkDescriptorSet> descriptorsets;


	UboArray 						CameraBuffer;


	DeletionQueue	deletorhandler;
	TextureBuilder texturehandler;
	RenderBuilder renderer;

};