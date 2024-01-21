#pragma once

#include "vkdefines.h"
#include "vkdevices.h"
#include "vkrenderer.h"
#include "vkdescriptors.h"
#include <fstream>
#include <iostream>

struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelinelayout;
};

struct MeshPushConstants {
	glm::vec4 data;
	glm::mat4 render_matrix;
};

class PipelineBuilder {
public:
	void 									init(DeviceBuilder* device, RenderBuilder& renderhandler, DescriptorBuilder* desc, DeletionQueue* deletor) 
											{ devicehandler = device; renderer = renderhandler; deletorhandler = deletor; descriptors = desc;};
	bool 									loadshader(const char* filepath, VkShaderModule* outshadermodule);

	void 									setuppipeline();
	void 									buildpipeline(bool check);
	void 									clearpipeline();
	void									recreatepipeline(bool check);

	VkPipeline& 							getpipeline() 	{ return pipeline; };
	DeviceBuilder*							getdevice()		{ return devicehandler; };
	RenderBuilder&							getrenderer()	{ return renderer; };

	VkPipelineShaderStageCreateInfo 		pipelineshadercreateinfo(VkShaderStageFlagBits stage, VkShaderModule shadermodule);
	VkPipelineVertexInputStateCreateInfo 	vertexinputstagecreateinfo();
	VkPipelineInputAssemblyStateCreateInfo 	inputassemblycreateinput(VkPrimitiveTopology topology);
	VkPipelineRasterizationStateCreateInfo 	rasterezationcreateinfo(VkPolygonMode polygonmode);
	VkPipelineMultisampleStateCreateInfo 	multisamplingcreateinfo();
	VkPipelineColorBlendAttachmentState 	colorblendattachmentcreateinfo();
	VkPipelineLayoutCreateInfo 				pipelinelayoutcreateinfo();

	Material* 								creatematerial(VkPipeline pipeline, VkPipelineLayout layout,const std::string& name);
	Material* 	getmaterial(const std::string& name);

	void        cleanmaterials() {materials.clear();};
void getvertexdescription();

private:
	DeletionQueue*							deletorhandler;
	DeviceBuilder* 							devicehandler;
	RenderBuilder 							renderer;
	DescriptorBuilder*						descriptors;
	
	VertexInputDescription 					vertexdescription;

	VkPipeline pipeline;
	std::vector<VkPipelineShaderStageCreateInfo> shaderstages;
	VkPipelineVertexInputStateCreateInfo 	vertexinputinfo;
	VkPipelineInputAssemblyStateCreateInfo 	inputassembly;
	VkViewport 								viewport;
	VkRect2D 								scissor;
	VkPipelineRasterizationStateCreateInfo 	rasterizer;
	VkPipelineColorBlendAttachmentState 	colorblendattachment;
	VkPipelineMultisampleStateCreateInfo 	multisampling;
	VkPipelineLayout 						pipelinelayout;

	std::unordered_map
	<std::string,Material> 					materials;

};