#pragma once

#include "anthraxAI/gamemodules/modules.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/utils/parser.h"
#include "anthraxAI/utils/thread.h"

#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/camera.h"

#include <atomic>
#include <cstdint>
#include <string>
namespace Core
{

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            ~Scene() { if (GameObjects) delete GameObjects; if (GameModules) delete GameModules; }
            void Init();
            void InitModules();

            void Loop();
            
            void ExportObjectInfo(const Keeper::Objects* obj);
            void RenderScene(bool playmode);
            bool HasAnimation(uint32_t id) { if (GameModules) { return GameModules->HasAnimation(id); } return false; }
            void ReloadAnimation(uint32_t id, const std::string& s) { if (GameModules) { return GameModules->ReloadAnimation(id, s); }}        

            void SetCurrentScene(const std::string& str);
            Modules::ScenesMap& GetScenes() { return GameModules->GetSceneModules(); }

            Keeper::Camera& GetCamera() { return *EditorCamera; }
            const Keeper::Base* GetGameObjects() const { return GameObjects; }
            void ReloadResources();
            void ParseSceneNames();
            const std::vector<std::string>& GetSceneNames() const { return SceneNames; }
                
            void KeepEditor(bool keep) { HasEditor = keep; }
            const std::string& GetCurrentScene() const { return CurrentScene; }
            void SetSelectedID(uint32_t id) { GameObjects->SetSelectedID(id); }
            uint32_t GetSelectedID() { return GameObjects->GetSelectedID(); }

        private:
            void PopulateModules();

            void LoadScene(const std::string& filename);
            void Render(Modules::Module& module);

            Keeper::Base* GameObjects = nullptr;
            Modules::Base* GameModules = nullptr;

            std::string CurrentScene = "intro";
            std::vector<Keeper::Info> ParsedSceneInfo;

            std::vector<std::string> SceneNames;
            
            Keeper::Camera* EditorCamera;

            Utils::Parser Parse;
            
            bool HasEditor = false;
            bool HasGBuffer = false;
            bool HasFrameGizmo = false;
            bool HasFrameOutline = false;
            bool HasFrameGrid = false;

    };

}
