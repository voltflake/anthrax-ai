#pragma once

#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/gfx/vkdefines.h"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

namespace Gfx
{
    class ShadercIncluder : public shaderc::CompileOptions::IncluderInterface
	{
        struct Data {
            char* buffer1;
            char* buffer2;
        };
	    public:
		    shaderc_include_result* GetInclude(const char* requested_src, shaderc_include_type type, const char* requesting_src, size_t include_depth) override;

		    void ReleaseInclude(shaderc_include_result* data) override;
	};

    struct VertexInputDescription {
        std::vector<VkVertexInputBindingDescription> Bindings;
        std::vector<VkVertexInputAttributeDescription> Attributes;
        VkPipelineVertexInputStateCreateFlags Flags = 0;
    };

    class Pipeline : public Utils::Singleton<Pipeline>
    {
        public:
            Material* GetMaterial(const std::string& name);
            void Build();

            void CompileShader(const std::string& name, shaderc_shader_kind kind, std::string& data);

        private:
        	VkPipeline Pipeline;
	        VkPipelineLayout PipelineLayout;

        	std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
	        VertexInputDescription 					VertexDescription;
            VkPipelineVertexInputStateCreateInfo 	VertexInputInfo;
            VkPipelineInputAssemblyStateCreateInfo 	InputAssembly;
            VkViewport 								Viewport;
            VkRect2D 								Scissor;
            VkPipelineRasterizationStateCreateInfo 	Rasterizer;
            VkPipelineColorBlendAttachmentState 	ColorBlendAttachment;
            VkPipelineMultisampleStateCreateInfo 	Multisampling;
            VkPipelineDepthStencilStateCreateInfo 	DepthStencil;

            void BuildMaterial(const std::string& material, VkShaderModule* vertexshader, const std::string& vertname, VkShaderModule* fragshader, const std::string& fragname, Gfx::RenderTargetsList id);
            bool LoadShader(const std::string& buffer, VkShaderModule* outshadermodule);

            bool LoadShader(const char* filepath, VkShaderModule* outshadermodule);
            Material* CreateMaterial(VkPipeline pipelinew, VkPipelineLayout layout, const std::string& name);
            void Setup(Gfx::RenderTargetsList id);
            void GetVertexDescription();
            VkPipelineVertexInputStateCreateInfo VertexInputStageCreateInfo();

            std::unordered_map<std::string,Material> Materials;
    };

}
