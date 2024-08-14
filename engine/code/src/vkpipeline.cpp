
#include "anthraxAI/vkpipeline.h"

bool PipelineBuilder::loadshader(const char* filepath, VkShaderModule* outshadermodule) {
	
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return false;
	}

    size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkShaderModule shadermodule;
	if (vkCreateShaderModule(devicehandler->getlogicaldevice(), &createInfo, nullptr, &shadermodule) != VK_SUCCESS) {
		std::cout << "failed to create shader module!\n";
		return false;
	}
	
	*outshadermodule = shadermodule;
	return true;
}

Material* PipelineBuilder::getmaterial(const std::string& name)
{
	auto it = materials.find(name);
	if (it == materials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


Material* PipelineBuilder::creatematerial(VkPipeline pipelinew, VkPipelineLayout layout, const std::string& name)
{
	Material mat;
	mat.pipelinewrite = pipelinew;
	mat.pipelinelayout = layout;
	materials[name] = mat;
	return &materials[name];
}

void PipelineBuilder::recreatepipeline(VkDevice device, bool check) {
	clearpipeline(device);
	vertexdescription.attributes.clear();
	vertexdescription.bindings.clear();
	shaderstages.clear();
	buildpipeline(check);
}

void PipelineBuilder::clearpipeline(VkDevice device) {
	vkDestroyPipelineLayout(device, pipelayoutsread, nullptr);
	vkDestroyPipeline(device, pipelineread, nullptr);
	
	// for (int i = 0; i < 3; ++i) {
	// 	vkDestroyPipelineLayout(device, pipelayouts[i], nullptr);
	// 	vkDestroyPipeline(device, pipelineswrite[i], nullptr);
	// }
}

void PipelineBuilder::buildpipeline(bool check) {

	VkShaderModule fragshader;
	VkShaderModule fragshadercopy;
	VkShaderModule fragshadermodel;
	VkShaderModule fragshaderdebug;

	std::string fragshaderstr;
	if (check) {
		fragshaderstr = "./shaders/test.frag.spv";
		shaderstages.clear();
	}
	else {
		fragshaderstr = "./shaders/sprite.frag.spv";
	}

	if (!loadshader(fragshaderstr.c_str(), &fragshader) || !loadshader("./shaders/model.frag.spv", &fragshadermodel) || !loadshader("./shaders/copy.frag.spv", &fragshadercopy) || !loadshader("./shaders/debug.frag.spv", &fragshaderdebug)) {
		std::cout << "Error: fragment shader module" << std::endl;
	}
	else {
		std::cout << "Fragment shader successfully loaded --> " << fragshaderstr << std::endl;
	}

	VkShaderModule vertexshader;
	VkShaderModule vertexshaderanimated;
	VkShaderModule vertexshadercopy;
	VkShaderModule vertexshadermodel;
	VkShaderModule vertexshaderdebug;

	if (!loadshader("./shaders/animmodel.vert.spv", &vertexshaderanimated) || !loadshader("./shaders/sprite.vert.spv", &vertexshader)|| !loadshader("./shaders/model.vert.spv", &vertexshadermodel)|| !loadshader("./shaders/copy.vert.spv", &vertexshadercopy) || !loadshader("./shaders/debug.vert.spv", &vertexshaderdebug)) {
		std::cout << "Error: vertex shader module" << std::endl;
	}
	else {
		std::cout << "Vertex shader successfully loaded" << std::endl;
	}

// sprite pipeline

	VkPipelineLayoutCreateInfo pipelinelayoutinfo = pipelinelayoutcreateinfo();
	
	VkPushConstantRange push_constant;
	push_constant.offset = 0;
	push_constant.size = sizeof(MeshPushConstants);
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelinelayoutinfo.pPushConstantRanges = &push_constant;
	pipelinelayoutinfo.pushConstantRangeCount = 1;	

	VkDescriptorSetLayout setLayouts[] = { descriptors->getgloballayout(), descriptors->getsamplerlayout() };
	pipelinelayoutinfo.setLayoutCount = 2;
	pipelinelayoutinfo.pSetLayouts = setLayouts;

	int ind = 0;
	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelayouts[ind]), "failed to create pipeline layput!");

	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));

	if (!check) {
		vertexinputinfo = vertexinputstagecreateinfo();
	}
	inputassembly = inputassemblycreateinput(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)devicehandler->getswapchainextent().width;
	viewport.height = (float)devicehandler->getswapchainextent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = devicehandler->getwindowxtent();
printf("w: %d ----------------- h^ %d\n\n",scissor.extent.width, scissor.extent.height );
	rasterizer = rasterezationcreateinfo(VK_POLYGON_MODE_FILL);

	multisampling = multisamplingcreateinfo();

	colorblendattachment = colorblendattachmentcreateinfo();
	
	depthstencil = depthstencilcreateinfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
	
	setuppipeline(ind);
	creatematerial(pipelineswrite[ind], pipelayouts[ind], "defaultmesh");
	
// model pipeline
	VkDescriptorSetLayout setLayoutsmodel[] = { descriptors->getgloballayout(), descriptors->getsamplerlayout(), descriptors->getstoragelayout() };
	pipelinelayoutinfo.setLayoutCount = 3;
	pipelinelayoutinfo.pSetLayouts = setLayoutsmodel;
	
	ind = 1;
	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshadermodel));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshadermodel));

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelayouts[ind]), "failed to create pipeline layput!");

	setuppipeline(ind);
	creatematerial(pipelineswrite[ind], pipelayouts[ind], "monkey");

// animated model pipeline
	VkDescriptorSetLayout setLayoutsanim[] = { descriptors->getgloballayout(), descriptors->getsamplerlayout(), descriptors->getstoragelayout(), descriptors->getstoragelayout() };
	pipelinelayoutinfo.setLayoutCount = 4;
	pipelinelayoutinfo.pSetLayouts = setLayoutsanim;
	ind = 2;
	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshaderanimated));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshadermodel));

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelayouts[ind]), "failed to create pipeline layput!");

	setuppipeline(ind);
	creatematerial(pipelineswrite[ind], pipelayouts[ind], "animated");

// debug pipeline
	ind = 3;
	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshaderdebug));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshaderdebug));

	VkPipelineLayoutCreateInfo pipelinelayoutinfodebug = pipelinelayoutcreateinfo();
	pipelinelayoutinfodebug.pPushConstantRanges = &push_constant;
	pipelinelayoutinfodebug.pushConstantRangeCount = 1;	

	VkDescriptorSetLayout setLayoutsdebug[] = { descriptors->getgloballayout(), descriptors->getsamplerlayout() };
	pipelinelayoutinfodebug.setLayoutCount = 2;
	pipelinelayoutinfodebug.pSetLayouts = setLayoutsdebug;

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfodebug, nullptr, &pipelayouts[ind]), "failed to create pipeline layput!");

	setuppipeline(ind);
	creatematerial(pipelineswrite[ind], pipelayouts[ind], "debug");

// subpass 2
	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelayoutsread), "failed to create pipeline layput!");

	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshadercopy));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshadercopy));
	setuppipelineread();
//---------------------

	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshader, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshaderanimated, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshader, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshadermodel, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshadercopy, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshadermodel, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshaderdebug, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshaderdebug, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshadercopy, nullptr);
}

void PipelineBuilder::setuppipeline(int ind) {

// Pipeline for subpass 0

	VkPipelineViewportStateCreateInfo viewportstate = {};
	viewportstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportstate.pNext = nullptr;

	viewportstate.viewportCount = 1;
	viewportstate.pViewports = &viewport;
	viewportstate.scissorCount = 1;
	viewportstate.pScissors = &scissor;

	VkPipelineColorBlendStateCreateInfo colorblending = {};
	colorblending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblending.pNext = nullptr;

	colorblending.logicOpEnable = VK_FALSE;
	colorblending.logicOp = VK_LOGIC_OP_COPY;
	colorblending.attachmentCount = 1;
	colorblending.pAttachments = &colorblendattachment;
	colorblending.blendConstants[0] = 1.f;
	colorblending.blendConstants[1] = 1.f;
	colorblending.blendConstants[2] = 1.f;
	colorblending.blendConstants[3] = 1.f;

	VkGraphicsPipelineCreateInfo pipelineinfo = {};
	pipelineinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineinfo.pNext = nullptr;

	pipelineinfo.stageCount = shaderstages.size();
	pipelineinfo.pStages = shaderstages.data();
	pipelineinfo.pVertexInputState = &vertexinputinfo;
	pipelineinfo.pInputAssemblyState = &inputassembly;
	pipelineinfo.pViewportState = &viewportstate;
	pipelineinfo.pRasterizationState = &rasterizer;
	pipelineinfo.pMultisampleState = &multisampling;
	pipelineinfo.pColorBlendState = &colorblending;
	pipelineinfo.pDepthStencilState = &depthstencil;
	pipelineinfo.layout = pipelayouts[ind];
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.subpass = 0;

	VK_ASSERT(vkCreateGraphicsPipelines(devicehandler->getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipelineswrite[ind]), "failed to create write pipeline\n");
}


void PipelineBuilder::setuppipelineread() {

// Pipeline for subpass 1

	VkPipelineViewportStateCreateInfo viewportstate = {};
	viewportstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportstate.pNext = nullptr;

	viewportstate.viewportCount = 1;
	viewportstate.pViewports = &viewport;
	viewportstate.scissorCount = 1;
	viewportstate.pScissors = &scissor;

	VkPipelineColorBlendStateCreateInfo colorblending = {};
	colorblending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblending.pNext = nullptr;

	colorblending.logicOpEnable = VK_FALSE;
	colorblending.logicOp = VK_LOGIC_OP_COPY;
	colorblending.attachmentCount = 1;
	colorblending.pAttachments = &colorblendattachment;
	colorblending.blendConstants[0] = 1.f;
	colorblending.blendConstants[1] = 1.f;
	colorblending.blendConstants[2] = 1.f;
	colorblending.blendConstants[3] = 1.f;

	VkGraphicsPipelineCreateInfo pipelineinfo = {};
	pipelineinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineinfo.pNext = nullptr;

	pipelineinfo.stageCount = shaderstages.size();
	pipelineinfo.pStages = shaderstages.data();
	pipelineinfo.pVertexInputState = &vertexinputinfo;
	pipelineinfo.pInputAssemblyState = &inputassembly;
	pipelineinfo.pViewportState = &viewportstate;
	pipelineinfo.pRasterizationState = &rasterizer;
	pipelineinfo.pMultisampleState = &multisampling;
	pipelineinfo.pColorBlendState = &colorblending;
	pipelineinfo.pDepthStencilState = &depthstencil;
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.layout = pipelayoutsread;
	pipelineinfo.subpass = 1;
	
	VK_ASSERT(vkCreateGraphicsPipelines(devicehandler->getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipelineread), "failed to create read pipeline\n");
}

VkPipelineLayoutCreateInfo PipelineBuilder::pipelinelayoutcreateinfo() {

	VkPipelineLayoutCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	return info;
}

void PipelineBuilder::getvertexdescription()
{
	VkVertexInputBindingDescription mainBinding = {};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vertexdescription.bindings.push_back(mainBinding);

	VkVertexInputAttributeDescription positionAttribute = {};
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(Vertex, position);

	VkVertexInputAttributeDescription normalAttribute = {};
	normalAttribute.binding = 0;
	normalAttribute.location = 1;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, normal);

	VkVertexInputAttributeDescription colorAttribute = {};
	colorAttribute.binding = 0;
	colorAttribute.location = 2;
	colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	VkVertexInputAttributeDescription uvattr = {};
	uvattr.binding = 0;
    uvattr.location = 3;
    uvattr.format = VK_FORMAT_R32G32_SFLOAT;
    uvattr.offset = offsetof(Vertex, uv);

	VkVertexInputAttributeDescription weightattr = {};
	weightattr.binding = 0;
	weightattr.location = 4;
	weightattr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	weightattr.offset = offsetof(Vertex, weights);

	VkVertexInputAttributeDescription boneattr = {};
	boneattr.binding = 0;
	boneattr.location = 5;
	boneattr.format =  VK_FORMAT_R32G32B32A32_SINT;
	boneattr.offset = offsetof(Vertex, boneID);

	vertexdescription.attributes.push_back(positionAttribute);
	vertexdescription.attributes.push_back(normalAttribute);
	vertexdescription.attributes.push_back(colorAttribute);
	vertexdescription.attributes.push_back(uvattr);
	vertexdescription.attributes.push_back(weightattr);
	vertexdescription.attributes.push_back(boneattr);

}

VkPipelineVertexInputStateCreateInfo PipelineBuilder::vertexinputstagecreateinfo() {
	
	getvertexdescription();

	VkPipelineVertexInputStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.pVertexAttributeDescriptions = vertexdescription.attributes.data();
	info.vertexAttributeDescriptionCount = vertexdescription.attributes.size();

	info.pVertexBindingDescriptions = vertexdescription.bindings.data();
	info.vertexBindingDescriptionCount = vertexdescription.bindings.size();
	return info;
}

VkPipelineShaderStageCreateInfo PipelineBuilder::pipelineshadercreateinfo(VkShaderStageFlagBits stage, VkShaderModule shadermodule) {

	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = nullptr;

	info.stage = stage;
	info.module = shadermodule;
	info.pName = "main";
	return info;
}

VkPipelineInputAssemblyStateCreateInfo PipelineBuilder::inputassemblycreateinput(VkPrimitiveTopology topology) {
	
	VkPipelineInputAssemblyStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.topology = topology;
	info.primitiveRestartEnable = VK_FALSE;
	return info;
}

VkPipelineRasterizationStateCreateInfo PipelineBuilder::rasterezationcreateinfo(VkPolygonMode polygonmode) {
	
	VkPipelineRasterizationStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.depthClampEnable = VK_FALSE;
	info.rasterizerDiscardEnable = VK_FALSE;

	info.polygonMode = polygonmode;
	info.lineWidth = 1.0f;
	info.cullMode = VK_CULL_MODE_NONE;
	info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	info.depthBiasEnable = VK_FALSE;
	info.depthBiasConstantFactor = 0.0f;
	info.depthBiasClamp = 0.0f;
	info.depthBiasSlopeFactor = 0.0f;

	return info;
}

VkPipelineMultisampleStateCreateInfo PipelineBuilder::multisamplingcreateinfo() {
		
	VkPipelineMultisampleStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.sampleShadingEnable = VK_FALSE;
	info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	info.minSampleShading = 1.0f;
	info.pSampleMask = nullptr;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;
	return info;
}

VkPipelineColorBlendAttachmentState PipelineBuilder::colorblendattachmentcreateinfo() {

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	return colorBlendAttachment;
}

VkPipelineDepthStencilStateCreateInfo PipelineBuilder::depthstencilcreateinfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
{
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
    info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f; // Optional
    info.maxDepthBounds = 1.0f; // Optional
    info.stencilTestEnable = VK_FALSE;

    return info;
}
