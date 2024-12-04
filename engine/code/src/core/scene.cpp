#include "anthraxAI/core/scene.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/npc.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include "anthraxAI/gfx/model.h"
#include <algorithm>
#include <cstdio>
#include <vulkan/vulkan_core.h>


void Core::Scene::Render(const std::string& scene)
{
    if (RQScenes[scene].HasCameraBuffer) {
        Gfx::Renderer::GetInstance()->PrepareCameraBuffer(*EditorCamera);
    }
    if (RQScenes[scene].HasStorageBuffer) {
        Gfx::Renderer::GetInstance()->PrepareStorageBuffer();
    }
    
    for (Gfx::RenderObject& obj :  RQScenes[scene].RenderQueue) {
        if (!obj.IsVisible) continue;

        if (RQScenes[scene].BindlessType == Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER) {
            BindlessRange = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::BasicParams>(Gfx::BasicParams({ obj.BufferBind, obj.StorageBind, obj.TextureBind, }));
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

        // gizmo
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
        RenderScene();
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_PLAY)) {
        /*UpdateObjects();    */
        GameObjects->Update();

        RenderScene();
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_RESOURCE_RELOAD)) {
        ReloadResources();
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
    //LoadScene("scene1");
    //LoadScene("textures");
    ParseSceneNames();  

    GameObjects = new Keeper::Base;
    GameObjects->Create<Keeper::Camera>(new Keeper::Camera(Keeper::Camera::Type::EDITOR, {0.0f, 0.0f, 3.0f}));
    EditorCamera = GameObjects->Get<Keeper::Camera>(Keeper::Type::CAMERA);

    //GameObjects->Create(ParsedInfo);
    //GameObjects->Get<Keeper::Sprite>(Keeper::Type::SPRITE)->PrintInfo(); 
    //GameObjects->ObjectList.push_back(new Object<class T>);
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


    /*for (auto& it : SceneObjects) {*/
    /*    Core::SceneInfo info;*/
    /*    std::string tag = it.first;*/
    /**/
    /*    Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);*/
    /*    info.Attachments = attachments;*/
    /*    info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER;*/
    /*    info.RenderQueue = LoadResources(tag, it.second);*/
    /*    RQScenes[tag] = info;*/
    /**/
    /*    UpdateResources(RQScenes[tag]);*/
    /*}*/
    // {
    //     Core::SceneInfo info;
    //     std::string tag = "gizmo";

    //     Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
    //     info.Attachments = attachments;
    //     info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER;
    //     info.RenderQueue = LoadResources(tag, { Keeper::Info() });
    //     RQScenes[tag] = info;
 
    //     UpdateResources(RQScenes[tag]);
    // }
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
    HasFrameGizmo = false;
    if (RQScenes.find("gizmo") != RQScenes.end()) {
        HasFrameGizmo = true;
    }

    Engine::GetInstance()->ClearState(ENGINE_STATE_RESOURCE_RELOAD);
    Engine::GetInstance()->SetState(ENGINE_STATE_EDITOR);
    Core::SceneInfo scene;
    for (auto& it : GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetType() == Keeper::CAMERA) continue;
            std::string tag = CurrentScene;
            Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
            scene.Attachments = attachments;
            scene.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER;
            scene.RenderQueue.push_back(LoadResources(tag, info));
        }     
    }
    RQScenes[CurrentScene] = scene; 
    UpdateResources(RQScenes[CurrentScene]);

    {
        Core::SceneInfo info;
        std::string tag = "grid";

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER;
        info.RenderQueue = LoadResources(tag, { Keeper::Info() });
        RQScenes[tag] = info;
 
        UpdateResources(RQScenes[tag]);
    }

    HasFrameGrid = false;
    if (RQScenes.find("grid") != RQScenes.end()) {
        HasFrameGrid = true;
    }
}

Gfx::RenderObject Core::Scene::LoadResources(const std::string& tag, const Keeper::Objects* info)
{
    Gfx::RenderObject rqobj;
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
    if (tag == "gizmo") {
        Gfx::RenderObject rqobj;
        rqobj.IsGrid = false;
        rqobj.VertexBase = false;
        rqobj.ID = 1000;
        rqobj.Position = {0.0f};
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial("gizmo");
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture("dummy");
        rqobj.Model = Gfx::Model::GetInstance()->GetModel("axis.obj");
        rq.push_back(rqobj);
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

    bool is3d = false;
    for (Keeper::Info obj : info) {

        Gfx::RenderObject rqobj;
        rqobj.ID = obj.ID;
        rqobj.Position = obj.Position;
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(obj.Material);
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture(obj.Texture);
        if (obj.IsModel) {
            is3d = true;
            rqobj.Model = Gfx::Model::GetInstance()->GetModel(obj.Model);
        }
        else {
            rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(obj.Texture);
        }
        rq.push_back(rqobj);
    }

    if (is3d) {
        Gfx::RenderObject rqobj;

        // Gfx::RenderObject rqobj;
        // rqobj.IsGrid = false;
        // rqobj.VertexBase = false;
        // rqobj.ID = Gfx::OBJECT_GIZMO;
        // rqobj.Position = {0.0f};
        // rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial("models");
        // rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture("dummy");
        // rqobj.Model = Gfx::Model::GetInstance()->GetModel("axis.obj");
        // rqobj.IsVisible = false;
        // rq.push_back(rqobj);

        rqobj.Position = {0.0f};
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial("grid");
        rqobj.Texture =Gfx::Renderer::GetInstance()->GetTexture("dummy");
        rqobj.Mesh = nullptr;
        rqobj.IsGrid = true;
        rqobj.VertexBase = true;
        rqobj.IsVisible = true;
        rq.push_back(rqobj);
    }

    ASSERT(rq.empty(), "Render Queue is empty, you probably passed a wrong tag");
    return rq;
}

void Core::Scene::UpdateObjects()
{
    int selectedID = Gfx::Renderer::GetInstance()->GetSelectedID();

    std::vector<Gfx::RenderObject>& rq = RQScenes[CurrentScene].RenderQueue;
    auto gizmo_it = std::find_if(rq.begin(), rq.end(), [&](Gfx::RenderObject& object) { return (object.ID == Gfx::OBJECT_GIZMO); } );

    if (selectedID == 0) {
        if (gizmo_it != rq.end()) {
            gizmo_it->IsVisible = false;
        }
        return ;
    }

    auto selected_it = std::find_if(rq.begin(), rq.end(), [&, selectedID](Gfx::RenderObject& object) { return (object.ID == selectedID); } );
    
    if (selected_it != rq.end() && selected_it->ID != Gfx::OBJECT_GIZMO) {
        if (gizmo_it != rq.end()) {
            gizmo_it->GizmoID = selectedID;

            gizmo_it->Position = selected_it->Position;
            gizmo_it->Position.y += 5;
            gizmo_it->IsVisible = true;
        }
    }
    else if (selected_it != rq.end() ) {
        int id = selected_it->GizmoID;
        auto gizmo_handle_it = std::find_if(rq.begin(), rq.end(), [&, id](Gfx::RenderObject& object) { return (object.ID == id); } );
        gizmo_handle_it->Position.y += 0.1;
        selected_it->Position.y += 0.1;
        //gizmo_it->Position.y += 0.1;
    }

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

        // if (SceneObjects[scenename].empty()) {
        //     ParsedSceneInfo.reserve(10);
        //     //SceneObjects[scenename].reserve(10); // random for now, should do according to the parsed level
        // }
        ParsedSceneInfo.emplace_back(info);
       // SceneObjects[scenename].emplace_back(info);

        node = Parse.GetChild(texture, Utils::LEVEL_ELEMENT_OBJECT);
    printf("\n-----------------PARSED--------------------------");
    printf("\n[scene]: |%s|\n[id]: %d\n[position]: [x]: %f [y]: %f [z]: %f\n[material][name]: %s [frag]: %s [vert]: %s\n[texture][name]: %s\n[model][name]: %s\n", 
        scenename.c_str(), idi, xpos, ypos, zpos, info.Material.c_str(), info.Fragment.c_str(), info.Vertex.c_str(), info.Texture.c_str(), info.Model.c_str());
    printf("-------------------------------------------------\n");
    }

}


