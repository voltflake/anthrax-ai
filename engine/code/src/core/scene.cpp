#include "anthraxAI/core/scene.h"
#include "anthraxAI/engine.h"
#include "anthraxAI/gfx/renderhelpers.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"
#include <cstdio>

void Core::Scene::RenderScene()
{
    Gfx::Renderer::GetInstance()->StartFrame(RQScenes[CurrentScene].Attachments);

    if (RQScenes[CurrentScene].HasCameraBuffer) {
        Gfx::Renderer::GetInstance()->PrepareCameraBuffer(EditorCamera);
    }
   
    uint32_t range;
    Gfx::Renderer::GetInstance()->NullTmpBindings();
    for (Gfx::RenderObject& obj :  RQScenes[CurrentScene].RenderQueue) {
        if (RQScenes[CurrentScene].BindlessType == Gfx::BINDLESS_DATA_TEXTURE) {
            range = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::TextureParams>(Gfx::TextureParams({ obj.TextureBind, obj.BufferBind }));
        }
        else if (RQScenes[CurrentScene].BindlessType == Gfx::BINDLESS_DATA_CAM_BUFFER) { 
            range = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::CanBufferParams>(Gfx::CanBufferParams({ obj.BufferBind }));
        }
        if (RQScenes[CurrentScene].BindlessType != Gfx::BINDLESS_DATA_NONE) {
            Gfx::DescriptorsBase::GetInstance()->Build();
            obj.BindlessOffset = range;
        }
        if (obj.VertexBase) {
            Gfx::Renderer::GetInstance()->DrawSimple(obj);
        }
        else {
            Gfx::Renderer::GetInstance()->Draw(obj);
        }
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
        UpdateCameraDirection();
        UpdateCameraPosition();

        RenderScene();
    }
}

void Core::Scene::UpdateResources(Core::SceneInfo& info)
{
    for (Gfx::RenderObject& obj : info.RenderQueue) {
        switch (info.BindlessType) {
            case Gfx::BINDLESS_DATA_TEXTURE: {
                obj.TextureBind = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(obj.Texture->GetImageView(), *(obj.Texture->GetSampler()));
    	        obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
                info.HasCameraBuffer = true;
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
    SetCamera(CAMERA_EDITOR);
    GetCamera().SetPosition({0.0f, 0.0f, 3.0f});
    GetCamera().SetDirections();

    LoadScene("scene1");
    LoadScene("textures");
    
}

void Core::Scene::Update()
{
    {
        Core::SceneInfo info;
        std::string tag = "intro";

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_CAM_BUFFER;
        info.RenderQueue = LoadResources(tag, { Core::ObjectInfo() });
        RQScenes[tag] = info;
 
        UpdateResources(RQScenes[tag]);
    }


    for (auto& it : SceneObjects) {
        Core::SceneInfo info;
        std::string tag = it.first;

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_TEXTURE;
        info.RenderQueue = LoadResources(tag, it.second);
        RQScenes[tag] = info;

        UpdateResources(RQScenes[tag]);
    }
}

std::vector<Gfx::RenderObject> Core::Scene::LoadResources(const std::string& tag, const std::vector<Core::ObjectInfo>& info)
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

    bool is3d = false;
    for (Core::ObjectInfo obj : info) {

        Gfx::RenderObject rqobj;

        rqobj.Position = obj.Position;
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial(obj.Material);
        rqobj.Texture = Gfx::Renderer::GetInstance()->GetTexture(obj.Texture);
        if (obj.IsModel) {
            is3d = true;
            rqobj.Model = Gfx::Model::GetInstance()->GetModel("./models/" + obj.Model);
        }
        else {
            rqobj.Mesh = Gfx::Mesh::GetInstance()->GetMesh(obj.Texture);
        }
        rq.push_back(rqobj);
    }

    if (is3d) {
        Gfx::RenderObject rqobj;
        rqobj.Position = {0.0f};
        rqobj.Material = Gfx::Pipeline::GetInstance()->GetMaterial("grid");
        rqobj.Texture =Gfx::Renderer::GetInstance()->GetTexture("dummy");
        rqobj.Mesh = nullptr;
        rqobj.IsGrid = true;
        rqobj.VertexBase = true;
        rq.push_back(rqobj);
    }

    ASSERT(rq.empty(), "Render Queue is empty, you probably passed a wrong tag");
    return rq;
}

void Core::Scene::LoadScene(const std::string& filename)
{
    Parse.Clear();
    Parse.Load(filename);
     
    Core::ObjectInfo info;

    std::string scenename = Parse.GetRootElement(); 

    float xpos, ypos, zpos = 0.0f;
    std::string matname, textname, modname, frag, vert;
    int idi = -1;
    
    Utils::NodeIt node = Parse.GetChild(Parse.GetRootNode(), Utils::LEVEL_ELEMENT_OBJECT);
    while (Parse.IsNodeValid(node)) { 
        idi = Parse.GetElement<int>(node, Utils::LEVEL_ELEMENT_ID);

        Utils::NodeIt position = Parse.GetChild(node, Utils::LEVEL_ELEMENT_POSITION);
        xpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_X);
        ypos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Y);
        zpos = Parse.GetElement<float>(position, Utils::LEVEL_ELEMENT_Z);
        info.Position = { xpos, ypos, zpos };

        Utils::NodeIt material = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MATERIAL);
        info.Material = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_NAME);
        info.Fragment = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_FRAG);
        info.Vertex = Parse.GetElement<std::string>(material, Utils::LEVEL_ELEMENT_VERT);

        Utils::NodeIt texture = Parse.GetChild(node, Utils::LEVEL_ELEMENT_TEXTURE);
        info.Texture = Parse.GetElement<std::string>(texture, Utils::LEVEL_ELEMENT_NAME);

        Utils::NodeIt model = Parse.GetChild(node, Utils::LEVEL_ELEMENT_MODEL);
        if (Parse.IsNodeValid(model)) {
            info.Model = Parse.GetElement<std::string>(model, Utils::LEVEL_ELEMENT_NAME);
            info.IsModel = true;
        }

        if (SceneObjects[scenename].empty()) {
            SceneObjects[scenename].reserve(10); // random for now, should do according to the parsed level
        }
        SceneObjects[scenename].emplace_back(info);

        node = Parse.GetChild(texture, Utils::LEVEL_ELEMENT_OBJECT);
    }

    printf("\n-----------------PARSED--------------------------");
    printf("\n[scene]: |%s|\n[id]: %d\n[position]: [x]: %f [y]: %f [z]: %f\n[material][name]: %s [frag]: %s [vert]: %s\n[texture][name]: %s\n[model][name]: %s\n", 
        scenename.c_str(), idi, xpos, ypos, zpos, info.Material.c_str(), info.Fragment.c_str(), info.Vertex.c_str(), info.Texture.c_str(), info.Model.c_str());
    printf("-------------------------------------------------\n");
}


