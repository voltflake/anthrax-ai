#include "anthraxAI/core/scene.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/gfx/vkdescriptors.h"
#include "anthraxAI/gfx/vkpipeline.h"
#include "anthraxAI/gfx/vkmesh.h"

void Core::Scene::RenderScene()
{
    Gfx::Renderer::GetInstance()->StartFrame(Scenes[CurrentScene].Attachments);

    if (Scenes[CurrentScene].HasCameraBuffer) {
        Gfx::Renderer::GetInstance()->PrepareCameraBuffer();
    }
   
    uint32_t range;
    Gfx::Material* material = nullptr;
	Gfx::MeshInfo* mesh = nullptr;
    bool bindpipe, bindindex = false;

    for (Gfx::RenderObject& obj :  Scenes[CurrentScene].RenderQueue) {
        if (Scenes[CurrentScene].BindlessType == Gfx::BINDLESS_DATA_TEXTURE) {
            range = Gfx::DescriptorsBase::GetInstance()->AddRange<Gfx::TextureParams>(Gfx::TextureParams({ obj.TextureBind, obj.BufferBind }));
        }
        if (Scenes[CurrentScene].BindlessType != Gfx::BINDLESS_DATA_NONE) {
            Gfx::DescriptorsBase::GetInstance()->Build();
            obj.BindlessOffset = range;
        }
        if (obj.VertexBase) {
            Gfx::Renderer::GetInstance()->DrawSimple(obj);
        }
        else {
            bindpipe = material != obj.Material;
            bindindex = mesh != obj.Mesh;
            material = bindpipe ? obj.Material : material;
            mesh = bindindex ? obj.Mesh : mesh;
            Gfx::Renderer::GetInstance()->Draw(obj, bindpipe, bindindex);
        }
    }

    Gfx::Renderer::GetInstance()->EndFrame();
}

void Core::Scene::UpdateResources(Core::SceneInfo& info)
{
    for (Gfx::RenderObject& obj : info.RenderQueue) {
        if (info.BindlessType == Gfx::BINDLESS_DATA_TEXTURE) {
            obj.TextureBind = Gfx::DescriptorsBase::GetInstance()->UpdateTexture(obj.Texture->GetImageView(), *(obj.Texture->GetSampler()));
    	    obj.BufferBind = Gfx::DescriptorsBase::GetInstance()->UpdateBuffer(Gfx::DescriptorsBase::GetInstance()->GetCameraBuffer(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);         
            info.HasCameraBuffer = true;
            info.HasTexture = true;
        }
    }
}

void Core::Scene::Init()
{
    {
        Core::SceneInfo info;
        std::string tag = "intro";

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_NONE;
        info.RenderQueue = LoadResources(tag);
        Scenes[tag] = info;
 
        UpdateResources(Scenes[tag]);
        // CurrentScene = tag;
    }
    {
        Core::SceneInfo info;
        std::string tag = "sprite";

        Gfx::AttachmentFlags attachments = static_cast<Gfx::AttachmentFlags>(Gfx::AttachmentFlags::RENDER_ATTACHMENT_COLOR | Gfx::AttachmentFlags::RENDER_ATTACHMENT_DEPTH);
        info.Attachments = attachments;
        info.BindlessType = Gfx::BINDLESS_DATA_TEXTURE;
        info.RenderQueue = LoadResources(tag);
        Scenes[tag] = info;
 
        UpdateResources(Scenes[tag]);
        //CurrentScene = tag;
    }
}

std::vector<Gfx::RenderObject> Core::Scene::LoadResources(const std::string& tag)
{
    std::vector<Gfx::RenderObject> rq;

    Gfx::RenderObject test;
    if (tag == "sprite") {
        test.Position = {0.0f};
        test.Material = Gfx::Pipeline::GetInstance()->GetMaterial(tag);
        test.Texture = Gfx::Renderer::GetInstance()->GetTexture("placeholder.jpg");
        test.Mesh = Gfx::Mesh::GetInstance()->GetMesh("placeholder.jpg");
        rq.push_back(test);

        test.Position = {0.0f};
        test.Material = Gfx::Pipeline::GetInstance()->GetMaterial(tag);
        test.Texture = Gfx::Renderer::GetInstance()->GetTexture("kote-v-bote.jpg");
        test.Mesh = Gfx::Mesh::GetInstance()->GetMesh("kote-v-bote.jpg");
        rq.push_back(test);
    }
    if (tag == "intro") {
        test.Position = {0.0f};
        test.Material = Gfx::Pipeline::GetInstance()->GetMaterial(tag);
        test.Texture = nullptr;
        test.Mesh = nullptr;
        test.VertexBase = true;
        rq.push_back(test);
    }

    return rq;
}