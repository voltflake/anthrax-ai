#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkdefines.h"
#include <fstream>
#include <unordered_map>

namespace Gfx
{
    struct Material {
        VkPipelineLayout PipelineLayout;
        VkPipeline Pipeline;
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

            bool LoadShader(const char* filepath, VkShaderModule* outshadermodule);
            Material* CreateMaterial(VkPipeline pipelinew, VkPipelineLayout layout, const std::string& name);
            void Setup(int ind);
            void GetVertexDescription();
            VkPipelineVertexInputStateCreateInfo VertexInputStageCreateInfo();

            std::unordered_map<std::string,Material> Materials;
    };
}
