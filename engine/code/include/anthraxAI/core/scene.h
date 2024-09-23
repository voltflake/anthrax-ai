#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkcmdhelper.h"

namespace Core
{
    struct RenderObject {
	    Gfx::MeshInfo* Mesh;
	    Gfx::Material* Material;
	    VkDescriptorSet* Textureset;
    
	    Vector3<float> Position;
    };

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            void LoadResources();
            void RenderScene();
        
        private:
            std::vector<RenderObject> RenderQueue;

            Gfx::CommandBuffer Cmd;
    };
}