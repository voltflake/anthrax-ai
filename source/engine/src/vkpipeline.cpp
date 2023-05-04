
#include "../includes/vkpipeline.h"

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
	if (vkCreateShaderModule(devicehandler.getlogicaldevice(), &createInfo, nullptr, &shadermodule) != VK_SUCCESS) {
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

void PipelineBuilder::buildpipeline() {

	VkShaderModule fragshader;
	if (!loadshader("./shaders/simpleShader.frag.spv", &fragshader)) {
		std::cout << "Error: fragment shader module" << std::endl;
	}
	else {
		std::cout << "Fragment shader successfully loaded" << std::endl;
	}

	VkShaderModule vertexshader;
	if (!loadshader("./shaders/simpleShader.vert.spv", &vertexshader)) {
		std::cout << "Error: vertex shader module" << std::endl;
	}
	else {
		std::cout << "Vertex shader successfully loaded" << std::endl;
	}

	VkPipelineLayoutCreateInfo pipelinelayoutinfo = pipelinelayoutcreateinfo();
	VkPushConstantRange push_constant;

	push_constant.offset = 0;
	
	push_constant.size = sizeof(MeshPushConstants);
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pipelinelayoutinfo.pPushConstantRanges = &push_constant;
	pipelinelayoutinfo.pushConstantRangeCount = 1;	

	pipelinelayoutinfo.setLayoutCount = 1;
	pipelinelayoutinfo.pSetLayouts = &descriptors.getgloballayout();

	VK_ASSERT(vkCreatePipelineLayout(devicehandler.getlogicaldevice(), &pipelinelayoutinfo, nullptr, &pipelinelayout), "failed to create pipeline layput!");

	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
	shaderstages.push_back(pipelineshadercreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));

	vertexinputinfo = vertexinputstagecreateinfo();
	
	inputassembly = inputassemblycreateinput(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)WindowExtend.width;
	viewport.height = (float)WindowExtend.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = WindowExtend;

	rasterizer = rasterezationcreateinfo(VK_POLYGON_MODE_FILL);

	multisampling = multisamplingcreateinfo();

	colorblendattachment = colorblendattachmentcreateinfo();
	
	setuppipeline();
	creatematerial(pipeline, pipelinelayout, "defaultmesh");

	vkDestroyShaderModule(devicehandler.getlogicaldevice(), vertexshader, nullptr);
	vkDestroyShaderModule(devicehandler.getlogicaldevice(), fragshader, nullptr);

	deletorhandler.pushfunction([=]() {
		vkDestroyPipeline(devicehandler.getlogicaldevice(), pipeline, nullptr);
		vkDestroyPipelineLayout(devicehandler.getlogicaldevice(), pipelinelayout, nullptr);
	});
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
	pipelineinfo.layout = pipelinelayout;
	pipelineinfo.renderPass = renderer.getrenderpass();
	pipelineinfo.subpass = 0;
	pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
	
	if (vkCreateGraphicsPipelines(devicehandler.getlogicaldevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &pipeline) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
	}
}

VkPipelineLayoutCreateInfo PipelineBuilder::pipelinelayoutcreateinfo() {

	VkPipelineLayoutCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;

	//info.flags = 0;
	// info.setLayoutCount = 0;
	// info.pSetLayouts = VK_NULL_HANDLE;
	// info.pushConstantRangeCount = 0;
	// info.pPushConstantRanges = nullptr;
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
	colorBlendAttachment.blendEnable = VK_FALSE;
	return colorBlendAttachment;
}