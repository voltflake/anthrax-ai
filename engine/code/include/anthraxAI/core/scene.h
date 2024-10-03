#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/renderhelpers.h"

#include <unordered_map>

namespace Core
{
    struct SceneInfo {
        Gfx::AttachmentFlags Attachments;
        std::vector<Gfx::RenderObject> RenderQueue;
        Gfx::BindlessDataType BindlessType;

        bool HasCameraBuffer;
        bool HasTexture;
    };
    typedef std::unordered_map<std::string, Core::SceneInfo> SceneMap;

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            void Init();

            void UpdateResources(SceneInfo& info);
            std::vector<Gfx::RenderObject> LoadResources(const std::string& tag);
            void RenderScene();

            void SetCurrentScene(const std::string& str) { CurrentScene = str; }
            SceneMap& GetScenes() { return Scenes; }
        private:
            std::string CurrentScene = "intro"; 
            SceneMap Scenes;
    };
}