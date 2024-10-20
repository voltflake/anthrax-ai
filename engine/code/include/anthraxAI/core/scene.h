#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/core/camera.h"

#include <unordered_map>

namespace Core
{
    enum CameraInfo {
        CAMERA_EDITOR = 0,
    };
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

            void SetCamera(CameraInfo info) { CurrentCamera = info; }
            Camera& GetCamera() { if (CurrentCamera == CAMERA_EDITOR) return EditorCamera; return EditorCamera; }
            void UpdateCameraDirection() { if (CurrentCamera == CAMERA_EDITOR) EditorCamera.UpdateDirection(); }
            void UpdateCameraPosition() { if (CurrentCamera == CAMERA_EDITOR) EditorCamera.UpdateMovement(); }
        private:
            std::string CurrentScene = "intro";
            SceneMap Scenes;
            
            CameraInfo CurrentCamera;
            Camera EditorCamera;
    };
}