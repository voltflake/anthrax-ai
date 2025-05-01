#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
#include <map>

#include "anthraxAI/core/animator.h"
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/core/imguihelper.h"

namespace Modules
{
    struct Info {
        int ColorID;
        int DepthID;
        Gfx::InputAttachments IAttachments;
        Gfx::BindlessDataType BindlessType;
    };

    enum Update {
        RESOURCES,
        MATERIALS,
        RQ,
        TEXTURE_UI_MANAGER,
        SAMPLERS,
    };

    class Module
    {
        public:
            Module() {}
            Module(Info info);

            std::vector<Gfx::RenderObject>& GetRenderQueue() { return RenderQueue; }
            Gfx::BindlessDataType GetBindlessType() const { return BindlessType; }
            Gfx::InputAttachments GetIAttachments() const { return IAttachments; }

            bool GetCameraBuffer() const { return HasCameraBuffer; }
            bool GetStorageBuffer() const { return HasStorageBuffer; }
            bool GetGizmo() const { return HasGizmo; }
            const std::string& GetTag() const { return Tag; }

            void SetTag(std::string tag) { Tag = tag; }
            void SetTexture(bool set) { HasTexture = set; }
            void SetStorageBuffer(bool set) { HasStorageBuffer = set; }
            void SetCameraBuffer(bool set) { HasCameraBuffer = set; }
            void SetGizmo(bool gizmo) { HasGizmo = gizmo; }
            void AddRQ(Gfx::RenderObject obj) { RenderQueue.push_back(obj); }

            void SetRenderQueue(std::vector<Gfx::RenderObject>& rq) { RenderQueue = rq; }
        private:
            std::string Tag;
            Gfx::InputAttachments IAttachments;
            std::vector<Gfx::RenderObject> RenderQueue;
            Gfx::BindlessDataType BindlessType;

            int ColorID;
            int DepthID;

            bool HasCameraBuffer;
            bool HasStorageBuffer;
            bool HasTexture;
            bool HasGizmo;

    };
    typedef std::unordered_map<std::string, Module> ScenesMap;
    typedef std::vector<Gfx::RenderObject> RenderQueueVec;
    class Base
    {
        public:
            Base(Keeper::Base* objects);
            ~Base() { if (Animator) delete Animator; }
            void Clear();

            void Update(uint32_t update_type, bool force_update = false);

            void Populate(const std::string& key, Info scene, std::function<bool(Keeper::Type)> skip_type);
            void Populate(const std::string& key, Info scene, Keeper::Info info);

            Module& Get(const std::string& key) { return SceneModules[key]; }

            bool HasFrameOutline() const { return HasOutline; }
            void ReloadAnimation(uint32_t id, const std::string& s) { if (Animator) { Animator->Reload(id, s); } }
            bool HasAnimation(uint32_t id) { if (Animator) { return Animator->HasAnimation(id); } return false; }
            void SetRenderQueue(const std::string& key, RenderQueueVec& rq) { SceneModules[key].SetRenderQueue(rq); }

            void RestartAnimator();

            ScenesMap& GetSceneModules() { return SceneModules; }
            void SetCurrentScene(const std::string& str) { CurrentScene = str; }

        private:
            bool HasOutline = false;
            std::string CurrentScene;
            ScenesMap SceneModules;
            Keeper::Base* GameObjects = nullptr;
            Core::AnimatorBase* Animator = nullptr;

            Gfx::RenderObject LoadResources(const Keeper::Objects* info);
            void UpdateResources();
            void UpdateResource(Modules::Module& module, Gfx::RenderObject& obj);
            void UpdateMaterials();
            void UpdateTexture(const std::string& str, Core::ImGuiHelper::TextureForUpdate upd);
            void UpdateTextureUIManager();
            void UpdateSamplers();
            void UpdateRQ();
            void ThreadedRQ(int i, Keeper::Objects* info);
    };

}
