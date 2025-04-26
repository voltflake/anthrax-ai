#include "anthraxAI/gamemodules/modules.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkdefines.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/utils/defines.h"

#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include <cstdint>
#include <functional>
#include <vector>
#include "tracy/Tracy.hpp"

void Modules::Base::Clear()
{
    SceneModules.clear();
}

void Modules::Base::Populate(const std::string& key, Modules::Info scene, std::function<bool(Keeper::Type)> skip_type)
{
    ASSERT(!GameObjects, "GameObjects is nullptr!");

    Module module(scene);
    module.SetTag(key);
    for (auto& it : GameObjects->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (skip_type(info->GetType())) continue;
            if (info->GetType() == Keeper::NPC) {
                module.SetGizmo(true);
            }
            module.AddRQ(LoadResources(info));
        }
    }
    SceneModules[key] = module;
}

void Modules::Base::Populate(const std::string& key, Modules::Info scene, Keeper::Info info)
{
    Module module(scene);

    module.SetTag(key);

    Gfx::RenderObject rqobj;
    rqobj.Position = {0.0f};
    rqobj.MaterialName = info.Material;
    rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(info.Material);
    if (info.Texture == "mask") {
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetRT(Gfx::GetKey(info.Texture));
    }
    if (info.Texture == "albedo") {
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetRT(Gfx::GetKey(info.Texture));
    }
    if (!info.Textures.empty()) {
        rqobj.Textures.resize(info.Textures.size());
        int i = 0;
        for (Gfx::RenderTarget*& rt : rqobj.Textures) {
            rt = Gfx::Renderer::GetInstance()->GetRT(Gfx::GetKey(info.Textures[i]));
            i++;
        }
    }
    /*else if (!info.Texture.empty()) {    */
    /*    rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture(info.Texture);*/
    /*}*/
    rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(info.Mesh);
    rqobj.VertexBase = info.VertexBase;
    rqobj.IsVisible = true;

    module.AddRQ(rqobj);

    SceneModules[key] = module;

    if (key == "mask" || key == "gbuffer") {
        Modules::RenderQueueVec rq = SceneModules[CurrentScene].GetRenderQueue();
        for (Gfx::RenderObject& obj : rq) {
            obj.MaterialName = key;
            obj.Material =  Gfx::Pipeline::GetInstance()->GetMaterial(obj.MaterialName);
        }
        SetRenderQueue(key, rq);
    }
}

void Modules::Base::RestartAnimator()
{
    if (Animator) {
        delete Animator;
    }
    Animator = new Core::AnimatorBase();

    Utils::Debug::GetInstance()->AnimStartMs = Engine::GetInstance()->GetTime();
    Animator->Init();

}

void Modules::Base::UpdateResource(Modules::Module& module, Gfx::RenderObject& obj)
{
    switch (module.GetBindlessType()) {
        case Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER: {
            for (int i = 0; i < MAX_FRAMES; i++) {
            if (!obj.Textures.empty()) {
                std::vector<Gfx::RenderTarget*>::iterator it = obj.Textures.begin();
                obj.TextureBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateTexture((*it)->GetImageView(), *((*it)->GetSampler()), (*it)->GetName(), i);
                it++;
                for (; it != obj.Textures.end(); ++it) {
                    ASSERT(!(*it), "Modules::Base::UpdateResource() invalid render target pointer!");
                    Gfx::DescriptorsBase::GetInstance()->UpdateTexture((*it)->GetImageView(), *((*it)->GetSampler()), (*it)->GetName(), i);
                }
            }
            else {
                ASSERT(!obj.Texture, "Modules::Base::UpdateResource() invalid render target pointer!");
                obj.TextureBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(obj.Texture->GetImageView(), *(obj.Texture->GetSampler()), obj.Texture->GetName(), i);
            }
    	    obj.BufferBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(i), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, i);
            obj.StorageBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetStorageBuffer(i), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, i);
            obj.InstanceBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetInstanceBuffer(i), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, i);
            }
            obj.HasStorage = obj.Model[0] ? true : false;
            module.SetCameraBuffer(true);
            module.SetStorageBuffer(true);
            module.SetTexture(true);

            break;
        }
        case Gfx::BINDLESS_DATA_CAM_BUFFER: {
            for (int i = 0; i < MAX_FRAMES; i++) {
    	    obj.BufferBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(i), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, i);
            }
            module.SetCameraBuffer(true);
            break;
        }
        default:
            break;
    }

}

void Modules::Base::UpdateResources()
{
    for (auto& it : SceneModules) {
        for (Gfx::RenderObject& obj : it.second.GetRenderQueue()) {
            UpdateResource(it.second, obj);
        }
    }
}

void Modules::Base::UpdateMaterials()
{
    for (auto& it : SceneModules) {
        for (Gfx::RenderObject& obj : it.second.GetRenderQueue()) {
            obj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(obj.MaterialName);
        }
    }
}

void Modules::Base::ThreadedRQ(int i, Keeper::Objects* info)
{
    SceneModules[CurrentScene].GetRenderQueue()[i].IsSelected = info->GetGizmo() || SceneModules[CurrentScene].GetRenderQueue()[i].ID == GameObjects->GetSelectedID() ? 1 : 0;
    SceneModules["mask"].GetRenderQueue()[i].IsSelected = SceneModules[CurrentScene].GetRenderQueue()[i].IsSelected;//info->GetGizmo() || SceneModules[CurrentScene].GetRenderQueue()[i].ID == GameObjects->GetSelectedID() ? 1 : 0;
    if (SceneModules["mask"].GetRenderQueue()[i].IsSelected) {
        HasOutline = true;
    }
    SceneModules[CurrentScene].GetRenderQueue()[i].IsVisible = info->IsVisible();
    SceneModules[CurrentScene].GetRenderQueue()[i].Position = info->GetPosition();
    if (HasAnimation(SceneModules[CurrentScene].GetRenderQueue()[i].ID)) {
        Animator->Update(SceneModules[CurrentScene].GetRenderQueue()[i]);
    }
}

void Modules::Base::UpdateRQ()
{
    if (GameObjects->IsValid(Keeper::Type::NPC)) {
        int i = 0;

        auto npc = GameObjects->Get(Keeper::Type::NPC);
        for (Keeper::Objects* info : npc) {
            if (Thread::Pool::GetInstance()->IsInit()) {
            Thread::Pool::GetInstance()->Push({
            Thread::Task::Name::UPDATE, Thread::Task::Type::EXECUTE, [this](int i, Keeper::Objects* info) {
                ThreadedRQ(i, info); }, {}, i, info, {} });
            }
            else {
                ThreadedRQ(i, info);
            }
            i++;
        }

        auto light = GameObjects->Get(Keeper::Type::LIGHT);
        for (Keeper::Objects* info : light) {
            SceneModules[CurrentScene].GetRenderQueue()[i].IsSelected = info->GetGizmo() || SceneModules[CurrentScene].GetRenderQueue()[i].ID == GameObjects->GetSelectedID() ? 1 : 0;
            SceneModules["mask"].GetRenderQueue()[i].IsSelected = SceneModules[CurrentScene].GetRenderQueue()[i].IsSelected;//info->GetGizmo() || SceneModules[CurrentScene].GetRenderQueue()[i].ID == GameObjects->GetSelectedID() ? 1 : 0;
            if (SceneModules["mask"].GetRenderQueue()[i].IsSelected) {
                HasOutline = true;
            }
            SceneModules[CurrentScene].GetRenderQueue()[i].IsVisible = info->IsVisible();
            SceneModules[CurrentScene].GetRenderQueue()[i].Position = info->GetPosition();
            i++;
        }
        i = 0;
        auto gizmo = GameObjects->Get(Keeper::Type::GIZMO);
        for (Keeper::Objects* info : gizmo) {
            SceneModules["gizmo"].GetRenderQueue()[i].IsVisible = info->IsVisible();
            SceneModules["gizmo"].GetRenderQueue()[i].Position = info->GetPosition();
            i++;
        }

        if (Gfx::Renderer::GetInstance()->GetUpdateSamplers()) {
            {
                Modules::Info info;
                info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
                info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
                info.IAttachments.Add(Gfx::RT_DEPTH, true);
                Populate("outline", info,
                    GameObjects->GetInfo(Keeper::Infos::INFO_OUTLINE)
                );

                for (Gfx::RenderObject& obj : SceneModules["outline"].GetRenderQueue()) {
                    UpdateResource(SceneModules["outline"], obj);
                }
            }
            {
                Modules::Info info;
                info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
                info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
                Populate("lighting", info,
                    GameObjects->GetInfo(Keeper::Infos::INFO_LIGHTING)
                );

                for (Gfx::RenderObject& obj : SceneModules["lighting"].GetRenderQueue()) {
                    UpdateResource(SceneModules["lighting"], obj);
                }
            }

            Gfx::Renderer::GetInstance()->SetUpdateSamplers(false);
        }
    }
}

void Modules::Base::UpdateTexture(const std::string& str, Core::ImGuiHelper::TextureForUpdate upd)
{
    int id = upd.ID;
    auto it = std::find_if(SceneModules[str].GetRenderQueue().begin(), SceneModules[str].GetRenderQueue().end(), [id](Gfx::RenderObject& obj) { return obj.ID == id; });
    if (it != SceneModules[str].GetRenderQueue().end()) {
        it->Texture = Gfx::Renderer::GetInstance()->GetTexture(upd.NewTextureName);
        it->TextureName = upd.NewTextureName;
        for (int i = 0; i < MAX_FRAMES; i++) {
        it->TextureBind[i] = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(it->Texture->GetImageView(), *(it->Texture->GetSampler()), it->Texture->GetName(), i);
        }
    }
}

void Modules::Base::UpdateTextureUIManager()
{
    if (Core::ImGuiHelper::GetInstance()->TextureNeedsUpdate()) {
        Core::ImGuiHelper::TextureForUpdate upd = Core::ImGuiHelper::GetInstance()->GetTextureForUpdate();
        UpdateTexture(CurrentScene, upd);
        UpdateTexture("gbuffer", upd);
        Core::ImGuiHelper::GetInstance()->ResetTextureUpdate();
    }
}

void Modules::Base::Update(uint32_t update_type)
{
    switch (update_type)
    {
        case Modules::Update::RESOURCES:
            UpdateResources();
            break;
        case Modules::Update::MATERIALS:
            UpdateMaterials();
            break;
        case Modules::Update::RQ:
            UpdateRQ();
            break;
        case Modules::Update::TEXTURE_UI_MANAGER:
            UpdateTextureUIManager();
            break;
        default:
            break;
    }
}

Gfx::RenderObject Modules::Base::LoadResources(const Keeper::Objects* info)
{
    Gfx::RenderObject rqobj;
    if (info->GetAxis() != -1) {
       rqobj.GizmoType = info->GetAxis();
    }
    rqobj.ID = info->GetID();
    rqobj.IsVisible = info->IsVisible();
    rqobj.Position = info->GetPosition();
    rqobj.MaterialName = info->GetMaterialName();
    rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(info->GetMaterialName());
    rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture(info->GetTextureName());
    rqobj.TextureName = info->GetTextureName();
    if (!info->GetModelName().empty()) {
        for (int i = 0; i < MAX_FRAMES; i++) {
        rqobj.Model[i] = Gfx::Model::GetInstance()->GetModel(info->GetModelName());
        }
    }
    else {
        rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(info->GetTextureName());
    }
    return rqobj;
}

Modules::Base::Base(Keeper::Base* objects)
: GameObjects(objects)
{

}


Modules::Module::Module(Modules::Info info)
{
    BindlessType = info.BindlessType;
    IAttachments = info.IAttachments;
}
