
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


Material* PipelineBuilder::creatematerial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name)
{
	Material mat;
	mat.pipeline = pipeline;
	mat.pipelinelayout = layout;
	materials[name] = mat;
	return &materials[name];
}

void PipelineBuilder::recreatepipeline(bool check) {
	clearpipeline();
	vertexdescription.attributes.clear();
	vertexdescription.bindings.clear();
	shaderstages.clear();
	buildpipeline(check);
}

void PipelineBuilder::clearpipeline() {
	vkDestroyPipelineLayout(devicehandler->getlogicaldevice(), pipelinelayout, nullptr);
	vkDestroyPipeline(devicehandler->getlogicaldevice(), pipelinesprite, nullptr);
	vkDestroyPipelineLayout(devicehandler->getlogicaldevice(), pipelinelayoutmodel, nullptr);
	vkDestroyPipeline(devicehandler->getlogicaldevice(), pipelinemodel, nullptr);
	vkDestroyPipelineLayout(devicehandler->getlogicaldevice(), pipelinelayoutdebug, nullptr);
	vkDestroyPipeline(devicehandler->getlogicaldevice(), pipelinedebug, nullptr);
}

void PipelineBuilder::buildpipeline(bool check) {

	VkShaderModule fragshader;
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

	if (!loadshader(fragshaderstr.c_str(), &fragshader) || !loadshader("./shaders/model.frag.spv", &fragshadermodel)  || !loadshader("./shaders/debug.frag.spv", &fragshaderdebug)) {
		std::cout << "Error: fragment shader module" << std::endl;
	}
	else {
		std::cout << "Fragment shader successfully loaded --> " << fragshaderstr << std::endl;
	}

	VkShaderModule vertexshader;
	VkShaderModule vertexshadermodel;
	VkShaderModule vertexshaderdebug;

	if (!loadshader("./shaders/sprite.vert.spv", &vertexshader)|| !loadshader("./shaders/model.vert.spv", &vertexshadermodel) || !loadshader("./shaders/debug.vert.spv", &vertexshaderdebug)) {
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
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pipelinelayoutinfo.pPushConstantRanges = &push_constant;
	pipelinelayoutinfo.pushConstantRangeCount = 1;	

	VkDescriptorSetLayout setLayouts[] = { descriptors->getgloballayout(), descriptors->getsamplerlayout() };
	pipelinelayoutinfo.setLayoutCount = 2;
	pipelinelayoutinfo.pSetLayouts = setLayouts;

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelinelayout), "failed to create pipeline layput!");

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

	rasterizer = rasterezationcreateinfo(VK_POLYGON_MODE_FILL);

	multisampling = multisamplingcreateinfo();

	colorblendattachment = colorblendattachmentcreateinfo();
	
	depthstencil = depthstencilcreateinfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
	
	setuppipeline();
	creatematerial(pipelinesprite, pipelinelayout, "defaultmesh");
	
// model pipeline
	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshadermodel));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshadermodel));

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelinelayoutmodel), "failed to create pipeline layput!");

	setuppipelinemodel();
	creatematerial(pipelinemodel, pipelinelayoutmodel, "monkey");

// debug pipeline
	shaderstages.clear();
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshaderdebug));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshaderdebug));

	VkPipelineLayoutCreateInfo pipelinelayoutinfodebug = pipelinelayoutcreateinfo();
	pipelinelayoutinfodebug.pPushConstantRanges = &push_constant;
	pipelinelayoutinfodebug.pushConstantRangeCount = 1;	

	VkDescriptorSetLayout setLayoutsdebug[] = { descriptors->getgloballayout() };
	pipelinelayoutinfodebug.setLayoutCount = 1;
	pipelinelayoutinfodebug.pSetLayouts = setLayoutsdebug;

	VK_ASSERT(vkCreatePipelineLayout(devicehandler->getlogicaldevice(), &pipelinelayoutinfodebug, nullptr, &pipelinelayoutdebug), "failed to create pipeline layput!");

	setuppipelinedebug();
	creatematerial(pipelinedebug, pipelinelayoutdebug, "debug");

	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshader, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshader, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshadermodel, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshadermodel, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), vertexshaderdebug, nullptr);
	vkDestroyShaderModule(devicehandler->getlogicaldevice(), fragshaderdebug, nullptr);
}

void PipelineBuilder::setuppipeline() {

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
	pipelineinfo.layout = pipelinelayout;
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.subpass = 0;
	pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
	
	if (vkCreateGraphicsPipelines(devicehandler->getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipelinesprite) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
	}
}

void PipelineBuilder::setuppipelinemodel() {

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

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicstate{};
	dynamicstate.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicstate.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicstate.pDynamicStates = dynamicStates.data();

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
	pipelineinfo.pDynamicState = &dynamicstate;
	pipelineinfo.layout = pipelinelayout;
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.subpass = 0;
	pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
	
	if (vkCreateGraphicsPipelines(devicehandler->getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipelinemodel) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
	}
}

void PipelineBuilder::setuppipelinedebug() {

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
	pipelineinfo.layout = pipelinelayoutdebug;
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.subpass = 0;
	pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
	
	if (vkCreateGraphicsPipelines(devicehandler->getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipelinedebug) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
	}
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

	vertexdescription.attributes.push_back(positionAttribute);
	vertexdescription.attributes.push_back(normalAttribute);
	vertexdescription.attributes.push_back(colorAttribute);
	vertexdescription.attributes.push_back(uvattr);
	

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
