#pragma once

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/core/camera.h"
#include "anthraxAI/utils/parser.h"

#include <atomic>
#include <string>
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
    struct ObjectInfo {
        int ID = 0;
        Vector3<float> Position;
        std::string Material;
        std::string Vertex;
        std::string Fragment;
        std::string Texture;
        std::string Model;
        bool IsModel = false;
        
        inline static std::atomic_int ObjectCounter = 0;

        ObjectInfo() { ObjectCounter++; }
    };  
    
    typedef std::unordered_map<std::string, Core::SceneInfo> RQSceneMap;
    typedef std::unordered_map<std::string, std::vector<Core::ObjectInfo>> SceneObjectMap;

    class Scene : public Utils::Singleton<Scene>
    {
        public:
            void Init();
            void Update();

            void Loop();
                
            void UpdateResources(SceneInfo& info);
            std::vector<Gfx::RenderObject> LoadResources(const std::string& tag, const std::vector<Core::ObjectInfo>& info);
            void RenderScene();

            void SetCurrentScene(const std::string& str) { CurrentScene = str; }
            RQSceneMap& GetScenes() { return RQScenes; }
            SceneObjectMap& GetResources() { return SceneObjects; }

            void SetCamera(CameraInfo info) { CurrentCamera = info; }
            Camera& GetCamera() { if (CurrentCamera == CAMERA_EDITOR) return EditorCamera; return EditorCamera; }
            void UpdateCameraDirection() { if (CurrentCamera == CAMERA_EDITOR) EditorCamera.UpdateDirection(); }
            void UpdateCameraPosition() { if (CurrentCamera == CAMERA_EDITOR) EditorCamera.UpdateMovement(); }

        private:
            void LoadScene(const std::string& filename);

            std::string CurrentScene = "intro";
            RQSceneMap RQScenes;
            SceneObjectMap SceneObjects;
            
            CameraInfo CurrentCamera;
            Camera EditorCamera;

            Utils::Parser Parse;

            float FPS = 0;
            float DeltaMs;
    };
}
