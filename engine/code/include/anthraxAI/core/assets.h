#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
#include <map>

#include "anthraxAI/utils/defines.h"
#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gameobjects/gameobjects.h"

namespace Modules 
{
    struct Info {
        Gfx::AttachmentFlags Attachments;
        Gfx::BindlessDataType BindlessType;
    };

    enum Update {
        RESOURCES,
        MATERIALS,
        RQ,
        TEXTURE_UI_MANAGER,
    };

    class Module
    {
        public:
            Module() {}
            Module(Info info);
                
            std::vector<Gfx::RenderObject>& GetRenderQueue() { return RenderQueue; }
            Gfx::BindlessDataType GetBindlessType() const { return BindlessType; }
            Gfx::AttachmentFlags GetAttachments() const { return Attachments; }

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
            Gfx::AttachmentFlags Attachments;
            std::vector<Gfx::RenderObject> RenderQueue;
            Gfx::BindlessDataType BindlessType;

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
            
            void Clear();

            void Update(uint32_t update_type);
            
            void Populate(const std::string& key, Info scene, std::function<bool(Keeper::Type)> skip_type);
            void Populate(const std::string& key, Info scene, Keeper::Info info);

            Module& Get(const std::string& key) { return SceneModules[key]; }
        
            bool HasFrameOutline() const { return HasOutline; }

            void SetRenderQueue(const std::string& key, RenderQueueVec& rq) { SceneModules[key].SetRenderQueue(rq); }

            ScenesMap& GetSceneModules() { return SceneModules; }
            void SetCurrentScene(const std::string& str) { CurrentScene = str; }
        private:
            bool HasOutline = false;
            std::string CurrentScene;
            ScenesMap SceneModules;
            Keeper::Base* GameObjects = nullptr;

            Gfx::RenderObject LoadResources(const Keeper::Objects* info);
            void UpdateResources();
            void UpdateResource(Modules::Module& module, Gfx::RenderObject& obj);
            void UpdateMaterials();
            void UpdateTextureUIManager();
            void UpdateRQ();
    };
  
}
