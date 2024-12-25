#include "anthraxAI/core/scene.h"
#include "anthraxAI/core/animator.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/model.h"
#include "anthraxAI/utils/debug.h"
#include "anthraxAI/utils/parser.h"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vulkan/vulkan_core.h>


void Core::Scene::Render(const std::string& scene)
{
    if (RQScenes[scene].HasCameraBuffer) {
        Gfx::Renderer::GetInstance()->PrepareCameraBuffer(*EditorCamera);
    }
        
    for (Gfx::RenderObject& obj :  RQScenes[scene].RenderQueue) {
        if (!obj.IsVisible) continue;

        if (RQScenes[scene].BindlessType == Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER) {
            BindlessRange = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::BasicParams>(Gfx::BasicParams({ obj.BufferBind, obj.StorageBind, obj.InstanceBind, obj.TextureBind, }));
        }
        else if (RQScenes[scene].BindlessType == Gfx::BINDLESS_DATA_CAM_BUFFER) { 
            BindlessRange = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::CamBufferParams>(Gfx::CamBufferParams({ obj.BufferBind }));
        }
        if (RQScenes[scene].BindlessType != Gfx::BINDLESS_DATA_NONE) {
            Gfx::DescriptorsBase::GetInstance()->Build();
            obj.BindlessOffset = BindlessRange;
        }
        if (obj.VertexBase) {
            Gfx::Renderer::GetInstance()->DrawSimple(obj);
        }
        else { 
            Gfx::Renderer::GetInstance()->Draw(obj);
        }
    }
}

void Core::Scene::RenderScene()
{
    Gfx::Renderer::GetInstance()->BeginFrame();
    Gfx::Renderer::GetInstance()->PrepareInstanceBuffer();

    {
        // objects from map
        Gfx::Renderer::GetInstance()->StartRender(static_cast<Gfx::AttachmentFlags>(RQScenes[CurrentScene].Attachments | Gfx::AttachmentFlags::RENDER_ATTACHMENT_CLEAR));
        Render(CurrentScene);
        Gfx::Renderer::GetInstance()->EndRender();

        // gizmo
        if (HasFrameGizmo) {
            Gfx::Renderer::GetInstance()->StartRender(static_cast<Gfx::AttachmentFlags>(RQScenes["gizmo"].Attachments | Gfx::AttachmentFlags::RENDER_ATTACHMENT_LOAD));
            Render("gizmo");
            Gfx::Renderer::GetInstance()->EndRender();
        }
        
        // grid
        if (HasFrameGrid && Utils::Debug::GetInstance()->Grid) {
            Gfx::Renderer::GetInstance()->StartRender(static_cast<Gfx::AttachmentFlags>(RQScenes["grid"].Attachments | Gfx::AttachmentFlags::RENDER_ATTACHMENT_LOAD));
            Render("grid");
            Gfx::Renderer::GetInstance()->EndRender();
        }

        // ui
        Gfx::Renderer::GetInstance()->StartRender(static_cast<Gfx::AttachmentFlags>(static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH | Gfx::AttachmentFlags::RENDER_ATTACHMENT_LOAD)));
        Gfx::Renderer::GetInstance()->RenderUI();
        Gfx::Renderer::GetInstance()->EndRender();
    }

    Gfx::Renderer::GetInstance()->EndFrame();
}

void Core::Scene::Loop()
{   
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_EDITOR)) {
        Core::ImGuiHelper::GetInstance()->Render();
        if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_RESOURCE_RELOAD)) {
            ReloadResources();
        }
        RenderScene();
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_PLAY)) {
        if (RQScenes[CurrentScene].HasStorageBuffer) {
              Gfx::Renderer::GetInstance()->PrepareStorageBuffer();
        }
        GameObjects->Update();
        UpdateRQ();
        RenderScene();
    }
}

void Core::Scene::UpdateRQ()
{
    if (GameObjects->IsValid(Keeper::Type::NPC)) {
        int i = 0;
        auto npc = GameObjects->Get(Keeper::Type::NPC);
        for (Keeper::Objects* info : npc) {
            RQScenes[CurrentScene].RenderQueue[i].IsSelected = info->GetGizmo() ? 1 : 0;  
            RQScenes[CurrentScene].RenderQueue[i].IsVisible = info->IsVisible();
            RQScenes[CurrentScene].RenderQueue[i].Position = info->GetPosition();
            i++;
        }
        i = 0;
        auto gizmo = GameObjects->Get(Keeper::Type::GIZMO);
        for (Keeper::Objects* info : gizmo) {
            RQScenes["gizmo"].RenderQueue[i].IsVisible = info->IsVisible();
            RQScenes["gizmo"].RenderQueue[i].Position = info->GetPosition();
            i++;
        }
    }
}

void Core::Scene::UpdateResources(Core::SceneInfo& info)
{
    for (Gfx::RenderObject& obj : info.RenderQueue) {
        switch (info.BindlessType) {
            case Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER: {
                obj.TextureBind = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(obj.Texture->GetImageView(), *(obj.Texture->GetSampler()));
    	        obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
                obj.StorageBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetStorageBuffer(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
                obj.InstanceBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetInstanceBuffer(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
                obj.HasStorage = obj.Model ? true : false;

                info.HasCameraBuffer = true;
                info.HasStorageBuffer = true;
                info.HasTexture = true;
                break;
            }
            case Gfx::BINDLESS_DATA_CAM_BUFFER: {
    	        obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
                info.HasCameraBuffer = true;
                break;
            }
            default:
                break;
        }
    }
}

void Core::Scene::Init()
{
    ParseSceneNames();  

    GameObjects = new Keeper::Base;
    GameObjects->Create<Keeper::Camera>(new Keeper::Camera(Keeper::Camera::Type::EDITOR, {0.0f, 0.0f, 3.0f}));
    EditorCamera = reinterpret_cast<Keeper::Camera*>(*(GameObjects->Get(Keeper::Type::CAMERA).begin()));

}

void Core::Scene::Update()
{
    {
        Core::SceneInfo info;
        std::string tag = "intro";

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER;
        info.RenderQueue = LoadResources(tag, { Keeper::Info() });
        RQScenes[tag] = info;
        UpdateResources(RQScenes[tag]);
    }
}

void Core::Scene::ReloadResources()
{
    vkDeviceWaitIdle(Gfx::Device::GetInstance()->GetDevice());
    
    BindlessRange = 0;

    ParsedSceneInfo.clear();
    ParsedSceneInfo.reserve(10);
    LoadScene(CurrentScene);
    
    GameObjects->CleanIfNot(Keeper::Type::CAMERA);
    GameObjects->Create(ParsedSceneInfo);
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::Y), Keeper::Gizmo::Type::Y));
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::X), Keeper::Gizmo::Type::X));
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::Z), Keeper::Gizmo::Type::Z));
    if (Animator) {
        delete Animator;
    }
    Animator = new AnimatorBase();
    Utils::Debug::GetInstance()->AnimStartMs = Engine::GetInstance()->GetTime();

	Gfx::Mesh::GetInstance()->CleanAll();
	Gfx::Model::GetInstance()->CleanAll();

    Core::PipelineDeletor::GetInstance()->CleanAll();
	Gfx::Renderer::GetInstance()->CleanTextures();
	Gfx::DescriptorsBase::GetInstance()->CleanAll();

	Gfx::DescriptorsBase::GetInstance()->Init();
	Gfx::Renderer::GetInstance()->CreateTextures();
	Gfx::Pipeline::GetInstance()->Build();
    Gfx::Mesh::GetInstance()->CreateMeshes();
	Gfx::Model::GetInstance()->LoadModels();

//----
    Engine::GetInstance()->ClearState(ENGINE_STATE_RESOURCE_RELOAD);
    Engine::GetInstance()->SetState(ENGINE_STATE_EDITOR);
    Core::SceneInfo scene;
    bool is3d = false;
    for (auto& it : GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetType() == Keeper::CAMERA || info->GetType() == Keeper::GIZMO) continue;
            if (info->GetType() == Keeper::NPC) {
                is3d = true;
            }
            std::string tag = CurrentScene;
            Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
            scene.Attachments = attachments;
            scene.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER;
            scene.RenderQueue.push_back(LoadResources(tag, info));
        }
    }
    RQScenes[CurrentScene] = scene; 
    UpdateResources(RQScenes[CurrentScene]);

    if (is3d) {
        Core::SceneInfo gizmoscene;
        auto gizmo = GameObjects->Get(Keeper::Type::GIZMO);
        for (Keeper::Objects* info : gizmo) {
            std::string tag = "gizmo";
            Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
            gizmoscene.Attachments = attachments;
            gizmoscene.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER;
            gizmoscene.RenderQueue.push_back(LoadResources(tag, info));
        }
        RQScenes["gizmo"] = gizmoscene; 
        UpdateResources(RQScenes["gizmo"]);

        Core::SceneInfo info;
        std::string tag = "grid";
        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER;
        info.RenderQueue = LoadResources(tag, { Keeper::Info() });
        RQScenes[tag] = info;
        UpdateResources(RQScenes[tag]);
    }
    
    Animator->Init();

    HasFrameGrid = false;
    if (RQScenes.find("grid") != RQScenes.end()) {
        HasFrameGrid = true;
    }

    HasFrameGizmo = false;
    if (RQScenes.find("gizmo") != RQScenes.end()) {
        HasFrameGizmo = true;
    }

}

Gfx::RenderObject Core::Scene::LoadResources(const std::string& tag, const Keeper::Objects* info)
{
    Gfx::RenderObject rqobj;
    rqobj.ID = info->GetID();
    rqobj.IsVisible = info->IsVisible();
    rqobj.Position = info->GetPosition();
    rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(info->GetMaterialName());
    rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture(info->GetTextureName());
    if (!info->GetModelName().empty()) {
        rqobj.Model = Gfx::Model::GetInstance()->GetModel(info->GetModelName());
    }
    else {
        rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(info->GetTextureName());
    }
    return rqobj;
}

std::vector<Gfx::RenderObject> Core::Scene::LoadResources(const std::string& tag, const std::vector<Keeper::Info>& info)
{
    std::vector<Gfx::RenderObject> rq;
    
    if (tag == "intro") {
        Gfx::RenderObject obj;
        obj.Position = {0.0f};
        obj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(tag);
        obj.Texture = nullptr;
        obj.Mesh = Gfx::Mesh::GetInstance()->GetMesh("dummy");;
        rq.push_back(obj);

        ASSERT(rq.empty(), "Render Queue is empty, you probably passed a wrong tag");
        return rq;
    }
    if (tag == "grid") {
        Gfx::RenderObject rqobj;
        rqobj.Position = {0.0f};
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial("grid");
        rqobj.Texture =Gfx::Renderer::GetInstance()->GetTexture("dummy");
        rqobj.Mesh = nullptr;
        rqobj.IsGrid = true;
        rqobj.VertexBase = true;
        rqobj.IsVisible = true;
        rq.push_back(rqobj);
        return rq;
    }

    ASSERT(rq.empty(), "Render Queue is empty, you probably passed a wrong tag");
    return rq;
}

void Core::Scene::ParseSceneNames()
{
    std::string path = "scenes/";
    
    SceneNames.reserve(20);
    for (const auto& name : std::filesystem::directory_iterator(path)) {
        std::string str = name.path();
        std::string basename = str.substr(str.find_last_of("/\\") + 1);
        SceneNames.push_back(basename.c_str());
    }
}

void Core::Scene::SetCurrentScene(const std::string& str) 
{ 
    CurrentScene = str;
    Engine::GetInstance()->ClearState(ENGINE_STATE_PLAY);
    Engine::GetInstance()->ClearState(ENGINE_STATE_EDITOR);
    Engine::GetInstance()->SetState(ENGINE_STATE_RESOURCE_RELOAD); 
}

void Core::Scene::LoadScene(const std::string& filename)
{
    Parse.Clear();
    Parse.Load(filename);
     
    std::string scenename = Parse.GetRootElement(); 

    float xpos, ypos, zpos = 0.0f;
    std::string matname, textname, modname, frag, vert;
    int idi = -1;
    
    Utils::NodeIt node = Parse.GetChild(Parse.GetRootNode(), Utils::LEVEL_ELEMENT_OBJECT);
    while (Parse.IsNodeValid(node)) { 
        Keeper::Info info;
        idi = Parse.GetElement<int>(node, Utils::LEVEL_ELEMENT_ID, 0);

        Utils::NodeIt position = Parse.GetChild(node, Utils::LEVEL_ELEMENT_POSITION);
        xpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_X, 0.0);
        ypos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Y, 0.0);
        zpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Z, 0.0);
        info.Position = { xpos, ypos, zpos };

        Utils::NodeIt material = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MATERIAL);
        info.Material = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_NAME, "");
        info.Fragment = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_FRAG, "");
        info.Vertex = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_VERT, "");

        Utils::NodeIt texture = Parse.GetChild(node, Utils::LEVEL_ELEMENT_TEXTURE);
        info.Texture = Parse.GetElement<std::string>(texture, Utils::LEVEL_ELEMENT_NAME, "");

        Utils::NodeIt model = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MODEL);
        if (Parse.IsNodeValid(model)) {
            info.Model = Parse.GetElement<std::string>(model, Utils::LEVEL_ELEMENT_NAME, "");
            info.IsModel = true;
        }
      
        Utils::NodeIt anim = Parse.GetChild(model, Utils::LEVEL_ELEMENT_ANIMATION);
        info.Animations.reserve(10);
        while (Parse.IsNodeValidInRange(anim)) {
            std::string animstr = Parse.GetElement<std::string>(anim, Utils::LEVEL_ELEMENT_NAME, "");
           info.Animations.push_back(animstr);
            anim = Parse.GetChild(++anim, Utils::LEVEL_ELEMENT_ANIMATION);
        }
    
        ParsedSceneInfo.emplace_back(info);

        node = Parse.GetChild(texture, Utils::LEVEL_ELEMENT_OBJECT);
    printf("\n-----------------PARSED--------------------------");
    printf("\n[scene]: |%s|\n[id]: %d\n[position]: [x]: %f [y]: %f [z]: %f\n[material][name]: %s [frag]: %s [vert]: %s\n[texture][name]: %s\n[model][name]: %s\n", 
        scenename.c_str(), idi, xpos, ypos, zpos, info.Material.c_str(), info.Fragment.c_str(), info.Vertex.c_str(), info.Texture.c_str(), info.Model.c_str());
    printf("Parsed animations:\n");
    for (std::string s : info.Animations) {
            printf("%s\n", s.c_str());
        }
    printf("-------------------------------------------------\n");
    }

}


