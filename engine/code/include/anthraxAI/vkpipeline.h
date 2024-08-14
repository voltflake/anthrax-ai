#pragma once

#include "anthraxAI/vkdefines.h"
#include "anthraxAI/vkdevices.h"
#include "anthraxAI/vkrenderer.h"
#include "anthraxAI/vkdescriptors.h"
#include <fstream>
#include <iostream>

struct Material {
	VkPipelineLayout pipelinelayout;
	VkPipeline pipelinewrite;
};

struct MeshPushConstants {
	glm::mat4 rendermatrix;

	int objectID;
    int boneind = -1;
	int debug = 0;
	int debugbones = 0;
};

class PipelineBuilder {
public:
	void 									init(DeviceBuilder* device, RenderBuilder& renderhandler, DescriptorBuilder* desc, DeletionQueue* deletor) 
											{ devicehandler = device; renderer = renderhandler; deletorhandler = deletor; descriptors = desc;};
	bool 									loadshader(const char* filepath, VkShaderModule* outshadermodule);

	void 									setuppipeline(int ind);
	void 									setuppipelineread();

	void 									setuppipelinemodel();
	void 									setuppipelinedebug();
	void 									buildpipeline(bool check);
	void 									clearpipeline(VkDevice device);
	void									recreatepipeline(VkDevice device, bool check);

	VkPipeline& 							getreadpipeline() 	{ return pipelineread; };
	VkPipelineLayout& 						getreadpipelayout() 	{ return pipelayoutsread; };


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

	Material* 								creatematerial(VkPipeline pipelinew, VkPipelineLayout layout,const std::string& name);
	Material* 	getmaterial(const std::string& name);

	void        cleanmaterials() {materials.clear();};
	void 		getvertexdescription();

private:
	DeletionQueue*							deletorhandler;
	DeviceBuilder* 							devicehandler;
	RenderBuilder 							renderer;
	DescriptorBuilder*						descriptors;
	
	VertexInputDescription 					vertexdescription;

	VkPipeline pipelineread;
	VkPipeline pipelineswrite[3];
	VkPipelineLayout pipelayouts[3];
	VkPipelineLayout pipelayoutsread;

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
	// VkPipelineLayout 						pipelinelayout;
	// VkPipelineLayout 						pipelinelayoutmodel;
	// VkPipelineLayout 						pipelinelayoutdebug;

	std::unordered_map
	<std::string,Material> 					materials;

};