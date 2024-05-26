#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkdescriptors.h"
#include <fstream>
#include <iostream>

struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelinelayout;
};

struct MeshPushConstants {
	//glm::vec4 data;
	glm::mat4 render_matrix;
	int debugcollision;
};

class PipelineBuilder {
public:
	void 									init(DeviceBuilder* device, RenderBuilder& renderhandler, DescriptorBuilder* desc, DeletionQueue* deletor) 
											{ devicehandler = device; renderer = renderhandler; deletorhandler = deletor; descriptors = desc;};
	bool 									loadshader(const char* filepath, VkShaderModule* outshadermodule);

	void 									setuppipeline();
	void 									setuppipelinemodel();
	void 									buildpipeline(bool check);
	void 									clearpipeline();
	void									recreatepipeline(bool check);

	VkPipeline& 							getpipelinesprite() 	{ return pipelinesprite; };
	VkPipeline& 							getpipelinemodel() 	{ return pipelinemodel; };
	DeviceBuilder*							getdevice()		{ return devicehandler; };
	RenderBuilder&							getrenderer()	{ return renderer; };
	VkViewport&								getviewport() 	{return viewport; };

	VkPipelineDepthStencilStateCreateInfo 	depthstencilcreateinfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);
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
	void 		getvertexdescription();

private:
	DeletionQueue*							deletorhandler;
	DeviceBuilder* 							devicehandler;
	RenderBuilder 							renderer;
	DescriptorBuilder*						descriptors;
	
	VertexInputDescription 					vertexdescription;

	VkPipeline pipelinesprite;
	VkPipeline pipelinemodel;

	std::vector<VkPipelineShaderStageCreateInfo> shaderstages;
	std::vector<VkPipelineShaderStageCreateInfo> shaderstagesmodel;

	VkPipelineVertexInputStateCreateInfo 	vertexinputinfo;
	VkPipelineInputAssemblyStateCreateInfo 	inputassembly;
	VkViewport 								viewport;
	VkRect2D 								scissor;
	VkPipelineRasterizationStateCreateInfo 	rasterizer;
	VkPipelineColorBlendAttachmentState 	colorblendattachment;
	VkPipelineMultisampleStateCreateInfo 	multisampling;
	VkPipelineDepthStencilStateCreateInfo 	depthstencil;
	VkPipelineLayout 						pipelinelayout;
	VkPipelineLayout 						pipelinelayoutmodel;

	std::unordered_map
	<std::string,Material> 					materials;

};