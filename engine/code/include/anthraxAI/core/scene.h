#pragma once

#include "anthraxAI/core/animator.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/utils/parser.h"

#include "anthraxAI/core/animator.h"

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/camera.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <filesystem>
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

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            ~Scene() { if (GameObjects) delete GameObjects; if (Animator) delete Animator; }
            void Init();
            
            void LoadIntro();

            void Loop();
                
            void UpdateMaterials();
            void UpdateResources(SceneInfo& info);
            std::vector<Gfx::RenderObject> LoadResources(const std::string& tag, const std::vector<Keeper::Info>& info);
            Gfx::RenderObject LoadResources(const std::string& tag, const Keeper::Objects* info);
            void RenderScene(bool playmode);
std::vector<glm::mat4> UpdateAnimation(Gfx::RenderObject& object) { return Animator->Update(object); }
            bool HasAnimation(uint32_t id) { return Animator->HasAnimation(id); }

            void SetCurrentScene(const std::string& str);
            RQSceneMap& GetScenes() { return RQScenes; }

            Keeper::Camera& GetCamera() { return *EditorCamera; }
            const Keeper::Base* GetGameObjects() const { return GameObjects; }
            void ReloadResources();
            void ParseSceneNames();
            const std::vector<std::string>& GetSceneNames() const { return SceneNames; }
                
            const std::string& GetCurrentScene() const { return CurrentScene; }
            void SetSelectedID(uint32_t id) { GameObjects->SetSelectedID(id); }
            uint32_t GetSelectedID() { return GameObjects->GetSelectedID(); }

        private:
            void UpdateRQ();
            void UpdateUIRQ();

            void LoadScene(const std::string& filename);
            void Render(const std::string& scene);

            Keeper::Base* GameObjects = nullptr;
            Core::AnimatorBase* Animator = nullptr;

            std::string CurrentScene = "intro";
            RQSceneMap RQScenes;
            std::vector<Keeper::Info> ParsedSceneInfo;

            std::vector<std::string> SceneNames;
            
            Keeper::Camera* EditorCamera;

            Utils::Parser Parse;

            bool HasFrameGizmo = false;
            bool HasFrameGrid = false;

            uint32_t BindlessRange = 0;

    };
}
