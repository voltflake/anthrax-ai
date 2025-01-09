#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/core/windowmanager.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/core/deletor.h"
#include <cstdio>
#include <string>

VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo() {

	VkPipelineLayoutCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	return info;
}

VkPipelineShaderStageCreateInfo PipelineShaderCreateinfo(VkShaderStageFlagBits stage, VkShaderModule shadermodule) {

	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = nullptr;

	info.stage = stage;
	info.module = shadermodule;
	info.pName = "main";
	return info;
}

VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology) {
	
	VkPipelineInputAssemblyStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.topology = topology;
	info.primitiveRestartEnable = VK_FALSE;
	return info;
}

VkPipelineRasterizationStateCreateInfo RasterezationCreateInfo(VkPolygonMode polygonmode) {
	
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

VkPipelineMultisampleStateCreateInfo MultiSamplingCreateInfo() {
		
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

VkPipelineColorBlendAttachmentState ColorBlendAttachmentCreateInfo() {

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

VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
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

bool Gfx::Pipeline::LoadShader(const char* filepath, VkShaderModule* outshadermodule) {

	std::vector<char> buffer;
	Utils::ReadFile(filepath, buffer);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkShaderModule shadermodule;
	VK_ASSERT(vkCreateShaderModule(Gfx::Device::GetInstance()->GetDevice(), &createInfo, nullptr, &shadermodule), "failed to create shader module!");
	*outshadermodule = shadermodule;
	return true;
}

bool Gfx::Pipeline::LoadShader(const std::string& buffer, VkShaderModule* outshadermodule) {

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkShaderModule shadermodule;
	VK_ASSERT(vkCreateShaderModule(Gfx::Device::GetInstance()->GetDevice(), &createInfo, nullptr, &shadermodule), "failed to create shader module!");
	*outshadermodule = shadermodule;
	return true;
}

Gfx::Material* Gfx::Pipeline::GetMaterial(const std::string& name)
{
	auto it = Materials.find(name);
	if (it == Materials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}

Gfx::Material* Gfx::Pipeline::CreateMaterial(VkPipeline pipelinew, VkPipelineLayout layout, const std::string& name)
{
	Gfx::Material mat;
	mat.Pipeline = pipelinew;
	mat.PipelineLayout = layout;
	Materials[name] = mat;

    Core::PipelineDeletor::GetInstance()->Push([=, this]() {
        vkDestroyPipelineLayout(Gfx::Device::GetInstance()->GetDevice(), mat.PipelineLayout, nullptr);
    });
    Core::Deletor::GetInstance()->Push([=, this]() {
        vkDestroyPipeline(Gfx::Device::GetInstance()->GetDevice(), mat.Pipeline, nullptr);
	});

	return &Materials[name];
}

void Gfx::Pipeline::CompileShader(const std::string& name, shaderc_shader_kind kind, std::string& data) {

	std::vector<char> buffer;
	Utils::ReadFile(name, buffer);
	
	shaderc::Compiler compiler;
  	shaderc::CompileOptions options{};

	options.SetIncluder(std::make_unique<Gfx::ShadercIncluder>());

  	// Like -DMY_DEFINE=1
  	//options.AddMacroDefinition("MY_DEFINE", "1");

  	shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(buffer.data(), buffer.size(), kind, name.c_str(), options);

	ASSERT(module.GetCompilationStatus() != shaderc_compilation_status_success, "Gfx::Pipeline::CompileShader() " + module.GetErrorMessage());

	data = std::string(std::string((const char*)module.cbegin(), (const char*)module.cend()));
}

void Gfx::Pipeline::Build() 
{
    if (!VertexDescription.Bindings.empty()) {
        VertexDescription.Bindings.clear();
        VertexDescription.Attributes.clear();
    }
    
	VkShaderModule fragshader;
	VkShaderModule vertexshader;
// sprite pipeline
	VkPipelineLayoutCreateInfo pipelinelayoutinfo = PipelineLayoutCreateInfo();
	
	VkPushConstantRange push_constant;
	push_constant.offset = 0;
	push_constant.size = sizeof(MeshPushConstants);
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelinelayoutinfo.pPushConstantRanges = &push_constant;
	pipelinelayoutinfo.pushConstantRangeCount = 1;	

	VkDescriptorSetLayout setLayouts[] = {  Gfx::DescriptorsBase::GetInstance()->GetBindlessLayout(), Gfx::DescriptorsBase::GetInstance()->GetDescriptorSetLayout()};
	pipelinelayoutinfo.setLayoutCount = 2;
	pipelinelayoutinfo.pSetLayouts = setLayouts;

	VertexInputInfo = VertexInputStageCreateInfo();
	InputAssembly = InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = (float)Core::WindowManager::GetInstance()->GetScreenResolution().x;
	Viewport.height = (float)Core::WindowManager::GetInstance()->GetScreenResolution().y;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;

	Scissor.offset = { 0, 0 };
	Scissor.extent = { (uint32_t)Core::WindowManager::GetInstance()->GetScreenResolution().x, (uint32_t)Core::WindowManager::GetInstance()->GetScreenResolution().y };

	Rasterizer = RasterezationCreateInfo(VK_POLYGON_MODE_FILL);
	Multisampling = MultiSamplingCreateInfo();
	ColorBlendAttachment = ColorBlendAttachmentCreateInfo();
	DepthStencil = DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

	std::string shaderbuf;

    Core::Scene* scene = Core::Scene::GetInstance();
    for (auto& it : scene->GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetFragmentName().empty() || info->GetVertexName().empty() || info->GetMaterialName().empty()) continue;
    
            VK_ASSERT(vkCreatePipelineLayout(Gfx::Device::GetInstance()->GetDevice(), &pipelinelayoutinfo, nullptr, &PipelineLayout), "failed to create pipeline layput!");
    
            if (!ShaderStages.empty()) {
                ShaderStages.clear();
                vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), vertexshader, nullptr);
                vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), fragshader, nullptr);
            }
            std::string frag = "./shaders/" + info->GetFragmentName();
            std::string vert = "./shaders/" + info->GetVertexName();
			CompileShader(frag, shaderc_glsl_fragment_shader, shaderbuf);
			LoadShader(shaderbuf, &fragshader);
			shaderbuf.clear();
			CompileShader(vert, shaderc_glsl_vertex_shader, shaderbuf);
			LoadShader(shaderbuf, &vertexshader);
			shaderbuf.clear();

            ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
            ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));
            Setup(0);
            CreateMaterial(Pipeline, PipelineLayout, info->GetMaterialName());
        }
    }

// intro
    if (!ShaderStages.empty()) {
		ShaderStages.clear();
		vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), vertexshader, nullptr);
		vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), fragshader, nullptr);
	}

	std::string buf;
	CompileShader("./shaders/intro.frag", shaderc_glsl_fragment_shader, buf);
	LoadShader(buf, &fragshader);
	buf.clear();
	CompileShader("./shaders/sprite.vert", shaderc_glsl_vertex_shader, buf);
	LoadShader(buf, &vertexshader);

	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));
    
	VK_ASSERT(vkCreatePipelineLayout(Gfx::Device::GetInstance()->GetDevice(), &pipelinelayoutinfo, nullptr, &PipelineLayout), "failed to create pipeline layput!");
	
	Setup(0);
	CreateMaterial(Pipeline, PipelineLayout, "intro");

//grid
    ShaderStages.clear();
	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), vertexshader, nullptr);
	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), fragshader, nullptr);
	std::string frag = "./shaders/grid.frag";
    std::string vert = "./shaders/grid.vert";
	CompileShader(frag, shaderc_glsl_fragment_shader, shaderbuf);
	LoadShader(shaderbuf, &fragshader);
	shaderbuf.clear();
	CompileShader(vert, shaderc_glsl_vertex_shader, shaderbuf);
	LoadShader(shaderbuf, &vertexshader);
	shaderbuf.clear();
	
	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));
    
	VK_ASSERT(vkCreatePipelineLayout(Gfx::Device::GetInstance()->GetDevice(), &pipelinelayoutinfo, nullptr, &PipelineLayout), "failed to create pipeline layput!");
	
	Setup(0);
	CreateMaterial(Pipeline, PipelineLayout, "grid");

//gizmo
    ShaderStages.clear();
	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), vertexshader, nullptr);
	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), fragshader, nullptr);
	
	frag = "./shaders/model.frag";
    vert = "./shaders/model.vert";
	CompileShader(frag, shaderc_glsl_fragment_shader, shaderbuf);
	LoadShader(shaderbuf, &fragshader);
	shaderbuf.clear();
	CompileShader(vert, shaderc_glsl_vertex_shader, shaderbuf);
	LoadShader(shaderbuf, &vertexshader);
	shaderbuf.clear();
	
	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_VERTEX_BIT, vertexshader));
	ShaderStages.push_back(PipelineShaderCreateinfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragshader));
    
	VK_ASSERT(vkCreatePipelineLayout(Gfx::Device::GetInstance()->GetDevice(), &pipelinelayoutinfo, nullptr, &PipelineLayout), "failed to create pipeline layput!");
	
	Setup(0);
	CreateMaterial(Pipeline, PipelineLayout, "gizmo");

	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), vertexshader, nullptr);
	vkDestroyShaderModule(Gfx::Device::GetInstance()->GetDevice(), fragshader, nullptr);
    ShaderStages.clear();
}

void Gfx::Pipeline::Setup(int ind) {

	VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkFormat depthformat = VK_FORMAT_D32_SFLOAT;

	VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo{};
    pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipelineRenderingCreateInfo.colorAttachmentCount = 1;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = Gfx::Device::GetInstance()->GetSwapchainFormat();
    pipelineRenderingCreateInfo.depthAttachmentFormat = depthformat;

	VkPipelineViewportStateCreateInfo viewportstate = {};
	viewportstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportstate.pNext = nullptr;

	viewportstate.viewportCount = 1;
	viewportstate.pViewports = &Viewport;
	viewportstate.scissorCount = 1;
	viewportstate.pScissors = &Scissor;

	VkPipelineColorBlendStateCreateInfo colorblending = {};
	colorblending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblending.pNext = nullptr;
	colorblending.logicOpEnable = VK_FALSE;
	colorblending.logicOp = VK_LOGIC_OP_COPY;
	colorblending.attachmentCount = 1;
	colorblending.pAttachments = &ColorBlendAttachment;
	colorblending.blendConstants[0] = 1.f;
	colorblending.blendConstants[1] = 1.f;
	colorblending.blendConstants[2] = 1.f;
	colorblending.blendConstants[3] = 1.f;

	VkGraphicsPipelineCreateInfo pipelineinfo = {};
	pipelineinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineinfo.pNext = &pipelineRenderingCreateInfo;
	pipelineinfo.stageCount = ShaderStages.size();
	pipelineinfo.pStages = ShaderStages.data();
	pipelineinfo.pVertexInputState = &VertexInputInfo;
	pipelineinfo.pInputAssemblyState = &InputAssembly;
	pipelineinfo.pViewportState = &viewportstate;
	pipelineinfo.pRasterizationState = &Rasterizer;
	pipelineinfo.pMultisampleState = &Multisampling;
	pipelineinfo.pColorBlendState = &colorblending;
	pipelineinfo.pDepthStencilState = &DepthStencil;
	pipelineinfo.layout = PipelineLayout;
	pipelineinfo.renderPass = nullptr;// renderer.getrenderpass();
	pipelineinfo.subpass = 0;

	VK_ASSERT(vkCreateGraphicsPipelines(Gfx::Device::GetInstance()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr, &Pipeline), "failed to create write pipeline\n");
}

void Gfx::Pipeline::GetVertexDescription()
{
	VkVertexInputBindingDescription mainBinding = {};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VertexDescription.Bindings.push_back(mainBinding);
    VkVertexInputAttributeDescription instind = {};
    instind.binding = 0;
    instind.location = 0;
    instind.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    instind.offset = offsetof(Vertex, instanceind);

	VkVertexInputAttributeDescription positionAttribute = {};
	positionAttribute.binding = 0;
	positionAttribute.location = 1;
	positionAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	positionAttribute.offset = offsetof(Vertex, position);

	VkVertexInputAttributeDescription normalAttribute = {};
	normalAttribute.binding = 0;
	normalAttribute.location = 2;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, normal);

	VkVertexInputAttributeDescription colorAttribute = {};
	colorAttribute.binding = 0;
	colorAttribute.location = 3;
	colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	VkVertexInputAttributeDescription uvattr = {};
	uvattr.binding = 0;
    uvattr.location = 4;
    uvattr.format = VK_FORMAT_R32G32_SFLOAT;
    uvattr.offset = offsetof(Vertex, uv);
	

	 VkVertexInputAttributeDescription weightattr = {};
	 weightattr.binding = 0;
	 weightattr.location = 5;
	 weightattr.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	 weightattr.offset = offsetof(Vertex, weights);

	 VkVertexInputAttributeDescription boneattr = {};
	 boneattr.binding = 0;
	 boneattr.location = 6;
	 boneattr.format =  VK_FORMAT_R32G32B32A32_SINT;
	 boneattr.offset = offsetof(Vertex, boneID);

	VertexDescription.Attributes.push_back(instind);
	VertexDescription.Attributes.push_back(positionAttribute);
	VertexDescription.Attributes.push_back(normalAttribute);
	VertexDescription.Attributes.push_back(colorAttribute);
	VertexDescription.Attributes.push_back(uvattr);
	VertexDescription.Attributes.push_back(weightattr);
	VertexDescription.Attributes.push_back(boneattr);
}

VkPipelineVertexInputStateCreateInfo Gfx::Pipeline::VertexInputStageCreateInfo() {
	
	GetVertexDescription();
	VkPipelineVertexInputStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.pVertexAttributeDescriptions = VertexDescription.Attributes.data();
	info.vertexAttributeDescriptionCount = VertexDescription.Attributes.size();

	info.pVertexBindingDescriptions = VertexDescription.Bindings.data();
	info.vertexBindingDescriptionCount = VertexDescription.Bindings.size();
	return info;
}

shaderc_include_result* Gfx::ShadercIncluder::GetInclude(const char* requested_src, shaderc_include_type type, const char* requesting_src, size_t include_depth)
{
	std::string name("./shaders/" + std::string(requested_src));

	char *nameb = new char[name.size()];
	memcpy(nameb, name.c_str(), name.size());

	std::vector<char> contents;
	Utils::ReadFile(name, contents);

	char *contentb = new char[contents.size()];
	memcpy(contentb, contents.data(), contents.size());

	return new shaderc_include_result {
		nameb,
		name.size(),
		contentb,
		contents.size(),
		new Data { nameb, contentb }
	};
}

void Gfx::ShadercIncluder::ReleaseInclude(shaderc_include_result* data)
{
	Data* fdata = reinterpret_cast<Data*>(data->user_data);
	delete[] fdata->buffer1;
	delete[] fdata->buffer2;
	delete fdata;
}