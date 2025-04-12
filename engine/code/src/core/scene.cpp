#include "anthraxAI/core/scene.h"
#include "anthraxAI/core/audio.h"
#include "anthraxAI/core/imguihelper.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/gamemodules/modules.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/gizmo.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/model.h"
#include "anthraxAI/utils/debug.h"
#include <cstdio>
#include <ctime>
#include <string>
#include <vulkan/vulkan_core.h>

void Core::Scene::Render(Modules::Module& module)
{
    Gfx::Renderer::GetInstance()->DebugRenderName(module.GetTag());
    for (Gfx::RenderObject& obj : module.GetRenderQueue()) {
        if (!obj.IsVisible) continue;
        if (module.GetTag() == "mask" && !obj.IsSelected) {
            Gfx::Renderer::GetInstance()->IncInstanceInd(obj.Model->Meshes.size());
            continue;
        }
        if (obj.VertexBase) {
            Gfx::Renderer::GetInstance()->DrawSimple(obj);
        }
        else { 
            Gfx::Renderer::GetInstance()->Draw(obj);
        }
    }
    Gfx::Renderer::GetInstance()->EndRenderName();
}

void Core::Scene::RenderScene(bool playmode)
{
    if (Gfx::Renderer::GetInstance()->BeginFrame()) {
        if (GameModules->Get(CurrentScene).GetStorageBuffer()) {
              Gfx::Renderer::GetInstance()->PrepareStorageBuffer();
        }

        Gfx::Renderer::GetInstance()->PrepareInstanceBuffer();
        Gfx::Renderer::GetInstance()->PrepareCameraBuffer(*EditorCamera);
        {
            // used for intro
            if (!HasGBuffer) {
                Gfx::Renderer::GetInstance()->StartRender(GameModules->Get(CurrentScene).GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_CLEAR);
                Render(GameModules->Get(CurrentScene));
                Gfx::Renderer::GetInstance()->EndRender();
            }
            // objects from map
            Gfx::Renderer::GetInstance()->StartRender(GameModules->Get("gbuffer").GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_CLEAR);
            Render(GameModules->Get("gbuffer"));
            Gfx::Renderer::GetInstance()->EndRender();
            
            Gfx::Renderer::GetInstance()->StartRender(GameModules->Get("lighting").GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_CLEAR);
            Render(GameModules->Get("lighting"));
            Gfx::Renderer::GetInstance()->EndRender();

            if (HasFrameGrid && Utils::Debug::GetInstance()->Grid) {

                Gfx::Renderer::GetInstance()->StartRender(GameModules->Get("grid").GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_LOAD);
                Render(GameModules->Get("grid"));
                Gfx::Renderer::GetInstance()->EndRender();
            }

            Gfx::Renderer::GetInstance()->CopyImage(Gfx::RT_MAIN_COLOR, Gfx::RT_MAIN_DEBUG);
            // gizmo, outline
            if (playmode && HasFrameGizmo) {
                Gfx::Renderer::GetInstance()->ResetInstanceInd();
                Gfx::Renderer::GetInstance()->StartRender(GameModules->Get("mask").GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_CLEAR);
                Render(GameModules->Get("mask"));
                Gfx::Renderer::GetInstance()->EndRender();

                if (GameModules->HasFrameOutline()) {
                    Gfx::Renderer::GetInstance()->StartRender(GameModules->Get(CurrentScene).GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_LOAD);
                    Render(GameModules->Get("outline"));
                    Gfx::Renderer::GetInstance()->EndRender();
                }

                Gfx::Renderer::GetInstance()->StartRender(GameModules->Get("gizmo").GetIAttachments(), Gfx::AttachmentRules::ATTACHMENT_RULE_LOAD);
                Render(GameModules->Get("gizmo"));
                Gfx::Renderer::GetInstance()->EndRender();
            }

            // ui
            if (HasGBuffer) {
                Gfx::Renderer::GetInstance()->TransferLayoutsDebug();
            }
            Gfx::Renderer::GetInstance()->StartRender(GameModules->Get(CurrentScene).GetIAttachments(), static_cast<Gfx::AttachmentRules>(Gfx::AttachmentRules::ATTACHMENT_RULE_LOAD));
            Gfx::Renderer::GetInstance()->RenderUI();
            Gfx::Renderer::GetInstance()->EndRender();
        }

        Gfx::Renderer::GetInstance()->EndFrame();
    }
}

void Core::Scene::Loop()
{
    Core::Audio::GetInstance()->Play();
  
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_INTRO)) {
        RenderScene(false);
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_EDITOR)) {
        Core::ImGuiHelper::GetInstance()->Render();
        
        Thread::Pool::GetInstance()->Pause(true);
        
        RenderScene(false);
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_PLAY)) {
        if (HasEditor) {
            Core::ImGuiHelper::GetInstance()->Render();
        }
        Thread::Pool::GetInstance()->Pause(false);
        Thread::BeginTime(Thread::Task::Name::UPDATE, (double)Engine::GetInstance()->GetTime());

        GameObjects->Update();
        GameModules->Update(Modules::Update::RQ);

        Thread::EndTime(Thread::Task::Name::UPDATE, (double)Engine::GetInstance()->GetTime());
        Thread::PrintTime(Thread::Task::Name::UPDATE);

        Thread::BeginTime(Thread::Task::Name::RENDER, (double)Engine::GetInstance()->GetTime());
        RenderScene(true);
        Thread::EndTime(Thread::Task::Name::RENDER, (double)Engine::GetInstance()->GetTime());
        Thread::PrintTime(Thread::Task::Name::RENDER);

    }

    GameModules->Update(Modules::Update::TEXTURE_UI_MANAGER);
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_SHADER_RELOAD)) {
        GameModules->Update(Modules::Update::MATERIALS);
        Engine::GetInstance()->ClearState(ENGINE_STATE_SHADER_RELOAD);
    }
    if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_RESOURCE_RELOAD)) {
        ReloadResources();
    }
        
}

void Core::Scene::Init()
{
    ParseSceneNames();  

    GameObjects = new Keeper::Base;
    GameObjects->Create<Keeper::Camera>(new Keeper::Camera(Keeper::Camera::Type::EDITOR, {1.0f, 1.0f, 3.0f}));
    EditorCamera = reinterpret_cast<Keeper::Camera*>(*(GameObjects->Get(Keeper::Type::CAMERA).begin()));

}

void Core::Scene::InitModules()
{
    Thread::Pool::GetInstance()->Init(8);
    
    GameModules = new Modules::Base(GameObjects);
        
    Modules::Info info;
    info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER ;
    info.IAttachments.Add(Gfx::RT_MAIN_COLOR);

    GameModules->Populate("intro", info,
        GameObjects->GetInfo(Keeper::Infos::INFO_INTRO)
    );
    GameModules->Update(Modules::Update::RESOURCES);
    Core::Audio::GetInstance()->Load("Anthrax_Mastered.wav");
    Core::Audio::GetInstance()->SetVolume(0.0f);
}

void Core::Scene::ReloadResources()
{
    Thread::Pool::GetInstance()->Reload();

    ParsedSceneInfo.clear();
    ParsedSceneInfo.reserve(10);
    LoadScene(CurrentScene);
    
    GameObjects->CleanIfNot(Keeper::Type::CAMERA, true);

    GameObjects->Create(ParsedSceneInfo);
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::Y), Keeper::Gizmo::Type::Y));
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::X), Keeper::Gizmo::Type::X));
    GameObjects->Create<Keeper::Gizmo>(new Keeper::Gizmo(GameObjects->GetGizmoInfo(Keeper::Gizmo::Type::Z), Keeper::Gizmo::Type::Z));
    EditorCamera->SetPosition({1.0f, 1.0f, 3.0f});

    Gfx::Vulkan::GetInstance()->ReloadResources();
    Core::Audio::GetInstance()->ResetState();

    Engine::GetInstance()->ClearState(ENGINE_STATE_RESOURCE_RELOAD);
    Engine::GetInstance()->SetState(ENGINE_STATE_EDITOR);
     
    PopulateModules();
    
    GameObjects->UpdateObjectNames();
    Core::ImGuiHelper::GetInstance()->UpdateObjectInfo();
}

void Core::Scene::ParseSceneNames()
{
    std::string path = "scenes/";
    
    SceneNames.reserve(20);
    for (const auto& name : std::filesystem::directory_iterator(path)) {
        std::string str = name.path().string();
        std::string basename = str.substr(str.find_last_of("/\\") + 1);
        SceneNames.push_back(basename.c_str());
    }
}

void Core::Scene::PopulateModules()
{
    GameModules->Clear();
    GameModules->SetCurrentScene(CurrentScene);
   
    {
        Modules::Info info;
        info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
        info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
        info.IAttachments.Add(Gfx::RT_DEPTH, true);
        GameModules->Populate(CurrentScene, info,
            [](Keeper::Type t) { return t == Keeper::CAMERA || t == Keeper::GIZMO; }  
        );
    }
    
    bool npc = GameObjects->Find(Keeper::NPC);
    HasFrameGizmo = false;
    HasFrameGrid = false;
    HasGBuffer = false;
    if (npc) {
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
            info.IAttachments.AddA(Gfx::RT_ALBEDO);
            info.IAttachments.AddP(Gfx::RT_POSITION);
            info.IAttachments.AddN(Gfx::RT_NORMAL);
            info.IAttachments.Add(Gfx::RT_DEPTH, true);
            GameModules->Populate("gbuffer", info,
                GameObjects->GetInfo(Keeper::Infos::INFO_GBUFFER)
            );
            HasGBuffer = true;
        }
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
            info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
           // info.IAttachments.Add(Gfx::RT_DEPTH, true);
            GameModules->Populate("lighting", info,
                GameObjects->GetInfo(Keeper::Infos::INFO_LIGHTING)
            );
        }
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
            info.IAttachments.Add(Gfx::RT_MAIN_COLOR);

            GameModules->Populate("gizmo", info,
                [](Keeper::Type t) { return t != Keeper::GIZMO; }  
            );
        }
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER ;
            info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
            info.IAttachments.Add(Gfx::RT_DEPTH, true);

            GameModules->Populate("grid", info, 
                GameObjects->GetInfo(Keeper::Infos::INFO_GRID)
            );
        }
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
            info.IAttachments.Add(Gfx::RT_MASK);

            GameModules->Populate("mask", info, 
                GameObjects->GetInfo(Keeper::Infos::INFO_MASK)
            );
        }
        {
            Modules::Info info;
            info.BindlessType = Gfx::BINDLESS_DATA_CAM_STORAGE_SAMPLER ;
            info.IAttachments.Add(Gfx::RT_MAIN_COLOR);
            info.IAttachments.Add(Gfx::RT_DEPTH, true);
            GameModules->Populate("outline", info, 
                GameObjects->GetInfo(Keeper::Infos::INFO_OUTLINE)
            ); 
        } 
        
        HasFrameGrid = true;
        HasFrameGizmo = true;
    }
    GameModules->Update(Modules::Update::RESOURCES);

    GameModules->RestartAnimator();
}

void Core::Scene::SetCurrentScene(const std::string& str) 
{ 
    CurrentScene = str;
    Engine::GetInstance()->ClearState(ENGINE_STATE_PLAY);
    Engine::GetInstance()->ClearState(ENGINE_STATE_EDITOR);
    Engine::GetInstance()->SetState(ENGINE_STATE_RESOURCE_RELOAD); 
}

void Core::Scene::ExportObjectInfo(const Keeper::Objects* obj)
{
    std::string rootname = Parse.GetRootElement();
    
    Utils::NodeIt obj_node = Parse.GetChildByID(Parse.GetRootNode(), obj->GetParsedID());

    if (!Parse.IsNodeValid(obj_node)) {
        printf("Can't save file without ID Node!!!! ;p \n");
        return;
    }
    
    Utils::NodeIt obj_texture = Parse.GetChild(obj_node, Utils::LEVEL_ELEMENT_TEXTURE);
    Utils::NodeIt obj_textname = Parse.GetChild(obj_texture, Utils::LEVEL_ELEMENT_NAME);
    if (Parse.IsNodeValid(obj_textname)) {
        Parse.UpdateElement(obj_textname, obj->GetTextureName());
    }
    Utils::NodeIt obj_position = Parse.GetChild(obj_node, Utils::LEVEL_ELEMENT_POSITION);
    if (Parse.IsNodeValid(obj_position)) {
        Utils::NodeIt x = Parse.GetChild(obj_position, Utils::LEVEL_ELEMENT_X);
        Parse.UpdateElement(x, std::to_string(obj->GetPosition().x));
        Utils::NodeIt y = Parse.GetChild(obj_position, Utils::LEVEL_ELEMENT_Y);
        Parse.UpdateElement(y, std::to_string(obj->GetPosition().y));
        Utils::NodeIt z = Parse.GetChild(obj_position, Utils::LEVEL_ELEMENT_Z);
        Parse.UpdateElement(z, std::to_string(obj->GetPosition().z));
    }

    Parse.Write(CurrentScene);
}

void Core::Scene::LoadScene(const std::string& filename)
{
    Parse.Clear();
    Parse.Load(filename);
     
    std::string scenename = Parse.GetRootElement(); 

    float xpos, ypos, zpos = 0.0f;
    std::string matname, textname, modname, frag, vert, id;
    
    Utils::NodeIt node = Parse.GetChild(Parse.GetRootNode(), Utils::LEVEL_ELEMENT_OBJECT);
    while (Parse.IsNodeValid(node)) { 
        Keeper::Info info;
        id = Parse.GetElement<std::string>(node, Utils::LEVEL_ELEMENT_ID, "");
        info.ParsedID = id;

        Utils::NodeIt position = Parse.GetChild(node, Utils::LEVEL_ELEMENT_POSITION);
        if (Parse.IsNodeValid(position)) {
            xpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_X, 0.0);
            ypos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Y, 0.0);
            zpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Z, 0.0);
            info.Position = { xpos, ypos, zpos };
        }

        Utils::NodeIt spawn = Parse.GetChild(node, Utils::LEVEL_ELEMENT_SPAWN);
        if (Parse.IsNodeValid(spawn)) {
            xpos = Parse.GetElement<float>(spawn, Utils::LEVEL_ELEMENT_X, 0.0);
            ypos = Parse.GetElement<float>(spawn, Utils::LEVEL_ELEMENT_Y, 0.0);
            zpos = Parse.GetElement<float>(spawn, Utils::LEVEL_ELEMENT_Z, 0.0);
            int size = Parse.GetElement<float>(spawn, Utils::LEVEL_ELEMENT_AMOUNT, 0);
            info.Offset = { xpos, ypos, zpos };
            info.Spawn = true;
        }

        Utils::NodeIt material = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MATERIAL);
        if (Parse.IsNodeValid(material)) {
            info.Material = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_NAME, "");
            info.Fragment = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_FRAG, "");
            info.Vertex = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_VERT, "");
        }

        Utils::NodeIt texture = Parse.GetChild(node, Utils::LEVEL_ELEMENT_TEXTURE);
        info.Texture = Parse.GetElement<std::string>(texture, Utils::LEVEL_ELEMENT_NAME, "");

        Utils::NodeIt model = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MODEL);
        if (Parse.IsNodeValid(model)) {
            info.Model = Parse.GetElement<std::string>(model, Utils::LEVEL_ELEMENT_NAME, "");
            info.IsModel = true;
        }

        Utils::NodeIt light = Parse.GetChild(node, Utils::LEVEL_ELEMENT_LIGHT);
        if (Parse.IsNodeValid(light)) {
            info.Model = Parse.GetElement<std::string>(light, Utils::LEVEL_ELEMENT_NAME, "");
            info.IsLight = true;
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
    printf("\n[scene]: |%s|\n[id]: %s\n[position]: [x]: %f [y]: %f [z]: %f\n[material][name]: %s [frag]: %s [vert]: %s\n[texture][name]: %s\n[model][name]: %s\n", 
        scenename.c_str(), id.c_str(), xpos, ypos, zpos, info.Material.c_str(), info.Fragment.c_str(), info.Vertex.c_str(), info.Texture.c_str(), info.Model.c_str());
    printf("Parsed animations:\n");
    for (std::string s : info.Animations) {
            printf("%s\n", s.c_str());
        }
    printf("-------------------------------------------------\n");
    }

}


