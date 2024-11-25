#pragma once

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gameobjects/objects/camera.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/utils/parser.h"
#include "anthraxAI/gameobjects/gameobjects.h"

#include <atomic>
#include <string>
#include <unordered_map>

namespace Core
{
    struct SceneInfo {
        Gfx::AttachmentFlags Attachments;
        std::vector<Gfx::RenderObject> RenderQueue;
        Gfx::BindlessDataType BindlessType;

        bool HasCameraBuffer;
        bool HasStorageBuffer;
        bool HasTexture;
    };
        
    typedef std::unordered_map<std::string, Core::SceneInfo> RQSceneMap;
    typedef std::unordered_map<std::string, std::vector<Keeper::Info>> SceneObjectMap;

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            ~Scene() { if (GameObjects) delete GameObjects; }
            void Init();
            void Update();

            void Loop();
                
            void UpdateResources(SceneInfo& info);
            std::vector<Gfx::RenderObject> LoadResources(const std::string& tag, const std::vector<Keeper::Info>& info);
            void RenderScene();

            void SetCurrentScene(const std::string& str) { CurrentScene = str; }
            RQSceneMap& GetScenes() { return RQScenes; }
            SceneObjectMap& GetResources() { return SceneObjects; }

            //void SetCamera(CameraInfo info) { CurrentCamera = info; }
            Keeper::Camera& GetCamera() { return *EditorCamera; }
            Keeper::Base* GetGameObjects() const { return GameObjects; }
            void UpdateObjects();        

        private:
            void LoadScene(const std::string& filename);

            Keeper::Base* GameObjects = nullptr;

            std::string CurrentScene = "intro";
            RQSceneMap RQScenes;
            SceneObjectMap SceneObjects;
            std::vector<Keeper::Info> ParsedInfo;
            
            Keeper::Camera* EditorCamera;

            Utils::Parser Parse;
    };
}
