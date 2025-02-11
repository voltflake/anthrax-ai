#include "anthraxAI/core/assets.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/utils/defines.h"

#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include <cstdint>
#include <functional>

void Modules::Base::Clear()
{
    for (auto& it : SceneModules) {
        auto& module = it.second;
        for (Gfx::RenderObject& obj : module.GetRenderQueue()) {
            if (module.GetBindlessType() == Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER) {  
                Gfx::DescriptorsBase::GetInstance()->ResetRanges<Gfx::BasicParams>();
            }
            else if (module.GetBindlessType() == Gfx::BINDLESS_DATA_CAM_BUFFER) { 
                Gfx::DescriptorsBase::GetInstance()->ResetRanges<Gfx::CamBufferParams>();
            }
        }
    }

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
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetMaskRT();
    }
    else if (!info.Texture.empty()) {    
        Gfx::Renderer::GetInstance()->GetTexture(info.Texture);
    }
    rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(info.Mesh);
    rqobj.VertexBase = info.VertexBase;
    rqobj.IsVisible = true;
   
    module.AddRQ(rqobj);
    
    SceneModules[key] = module;

    if (key == "mask") {
        Modules::RenderQueueVec rq = SceneModules[CurrentScene].GetRenderQueue();
        for (Gfx::RenderObject& obj : rq) {
            obj.MaterialName = "mask";
            obj.Material =  Gfx::Pipeline::GetInstance()->GetMaterial(obj.MaterialName);
        }
        SetRenderQueue("mask", rq);
    }
}

void Modules::Base::UpdateResource(Modules::Module& module, Gfx::RenderObject& obj)
{
    uint32_t BindlessRange = 0;
    switch (module.GetBindlessType()) {
        case Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER: {
            obj.TextureBind = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(obj.Texture->GetImageView(), *(obj.Texture->GetSampler()));
    	    obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            obj.StorageBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetStorageBuffer(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            obj.InstanceBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetInstanceBuffer(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            obj.HasStorage = obj.Model ? true : false;
            module.SetCameraBuffer(true);
            module.SetStorageBuffer(true);
            module.SetTexture(true);
            
            BindlessRange = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::BasicParams>(Gfx::BasicParams({ obj.BufferBind, obj.StorageBind, obj.InstanceBind, obj.TextureBind, }));

            break;
        }
        case Gfx::BINDLESS_DATA_CAM_BUFFER: {
    	    obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            module.SetCameraBuffer(true);
            
            BindlessRange = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::CamBufferParams>(Gfx::CamBufferParams({ obj.BufferBind }));
            break;
        }
        default:
            break;
    }

    obj.BindlessOffset = BindlessRange;
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

void Modules::Base::UpdateRQ()
{
    if (GameObjects->IsValid(Keeper::Type::NPC)) {
        int i = 0;
        auto npc = GameObjects->Get(Keeper::Type::NPC);
        for (Keeper::Objects* info : npc) {
            
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
            Populate("outline", {
                .Attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH),
                .BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER }, 
                GameObjects->GetInfo(Keeper::Infos::INFO_OUTLINE)
            );
            for (Gfx::RenderObject& obj : SceneModules["outline"].GetRenderQueue()) {
                UpdateResource(SceneModules["outline"], obj);
            }
            /*GameModules->Get("outline").GetRenderQueue() = LoadResources("outline", { Keeper::Info() });*/
            /*UpdateResources(RQScenes["outline"]);*/
            Gfx::Renderer::GetInstance()->SetUpdateSamplers(false);
        }
    }
}

void Modules::Base::UpdateTextureUIManager()
{
    if (Core::ImGuiHelper::GetInstance()->TextureNeedsUpdate()) {
        Core::ImGuiHelper::TextureForUpdate upd = Core::ImGuiHelper::GetInstance()->GetTextureForUpdate();
        int id = upd.ID; 
        auto it = std::find_if(SceneModules[CurrentScene].GetRenderQueue().begin(), SceneModules[CurrentScene].GetRenderQueue().end(), [id](Gfx::RenderObject& obj) { return obj.ID == id; });
        if (it != SceneModules[CurrentScene].GetRenderQueue().end()) {
            it->Texture = Gfx::Renderer::GetInstance()->GetTexture(upd.NewTextureName);
            it->TextureName = upd.NewTextureName;
            it->TextureBind = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(it->Texture->GetImageView(), *(it->Texture->GetSampler()));
        }
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
        rqobj.Model = Gfx::Model::GetInstance()->GetModel(info->GetModelName());
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
    Attachments = info.Attachments;
    BindlessType = info.BindlessType;
}
