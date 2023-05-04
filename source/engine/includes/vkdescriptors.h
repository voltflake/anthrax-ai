#pragma once

#include "vkdefines.h"
#include "vkrenderer.h"
#include "vkbuffer.h"
#include "vktexture.h"

class DescriptorBuilder {
public:
	void init(RenderBuilder& rendererh, DeletionQueue& deletor, TextureBuilder& texture) { deletorhandler = deletor; renderer = rendererh; texturehandler = texture;};

	void builddescriptors();

	size_t paduniformbuffersize(size_t originalsize);

	VkDescriptorSetLayout& getgloballayout() { return globalsetlayout; };

	UboArray& 						getcamerabuffer() 		{return CameraBuffer;};

	std::vector<VkDescriptorSet>& getdescriptorset() { return descriptorsets;};

private:
	VkDescriptorSetLayout globalsetlayout;
	VkDescriptorPool descriptorpool;
	std::vector<VkDescriptorSet> descriptorsets;


	UboArray 						CameraBuffer;


	DeletionQueue	deletorhandler;
	TextureBuilder texturehandler;
	RenderBuilder renderer;

};