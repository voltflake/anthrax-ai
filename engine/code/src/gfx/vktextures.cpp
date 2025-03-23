#include "anthraxAI/core/imguihelper.h"
#include "anthraxAI/gameobjects/gameobjects.h"
#include "anthraxAI/gameobjects/objects/sprite.h"
#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include "anthraxAI/gfx/vkrendertarget.h"
#include <cstdio>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool Gfx::Renderer::CreateTextureFromInfo(const std::string& texturename)
{
    if (texturename.empty()) {
        return false;
    }
    std::string path = "./textures/";
    std::string texture = texturename;

    auto it = Textures.find(texture);
    if (it != Textures.end()) {
        return false;
    }
    Textures[texture] = CreateTexture(path + texture);
    CreateSampler(Textures[texture]);
    
    if (Core::ImGuiHelper::GetInstance()->IsInit()) {
        Textures[texture].SetImGuiDescriptor();
    }
    return true;
}

void Gfx::Renderer::CleanTextures()
{
    for (auto& it : Textures) {
        it.second.Clean();
    }
    Textures.clear();
}

void Gfx::Renderer::CreateTextures()
{
    Core::Scene* scene = Core::Scene::GetInstance();
    for (auto& it : scene->GetGameObjects()->GetObjects()) {
        for (Keeper::Objects* info : it.second) {
            if (info->GetTextureName().empty()) {
                continue;
            }
            if (!CreateTextureFromInfo(info->GetTextureName())) {
                continue;
            }
        }
    }
    
    // load others from texture folder
    //
    std::string path = "textures/";
    std::vector<std::string> names;
    names.reserve(20);
    for (const auto& name : std::filesystem::directory_iterator(path)) {
        std::string str = name.path().string();
        std::string basename = str.substr(str.find_last_of("/\\") + 1);
        bool exists = basename.find(".jpg") != std::string::npos || basename.find(".png") != std::string::npos;
        if (exists) {
            names.emplace_back(basename.c_str());
           // printf("BASENAME: %s\n", basename.c_str());
        }
    }
    
    for (const std::string& s : names) {
        if (!CreateTextureFromInfo(s)) {
            continue;
        }
    }
}

Gfx::RenderTarget Gfx::Renderer::CreateTexture(const std::string& path)
{
    RenderTarget texture;
    int width, height, channels;

    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize imagesize = width * height * 4;

    std::cout << path << '\n';
    ASSERT(!pixels, "failed to load texture image!");

    BufferHelper::Buffer stagingbuffer;
    BufferHelper::AllocateBuffer(stagingbuffer, imagesize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    BufferHelper::MapMemory(stagingbuffer, imagesize, 0, pixels);
   
    stbi_image_free(pixels);

    texture.SetFormat(VK_FORMAT_R8G8B8A8_SRGB);
    texture.SetDimensions({ width, height });

    texture.CreateRenderTarget();

    Submit([&](VkCommandBuffer cmd) {
        texture.MemoryBarrier(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    });
    Submit([&](VkCommandBuffer cmd) {
        texture.Copy(cmd, stagingbuffer.Buffer, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    });
    Submit([&](VkCommandBuffer cmd) {
        texture.MemoryBarrier(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });
    vkDestroyBuffer(Gfx::Device::GetInstance()->GetDevice(), stagingbuffer.Buffer, nullptr);
    vkFreeMemory(Gfx::Device::GetInstance()->GetDevice(), stagingbuffer.DeviceMemory, nullptr);

    return texture;
}

void Gfx::Renderer::CreateSampler(RenderTarget& rt)
{
    VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerinfo.pNext = nullptr;

	samplerinfo.magFilter = VK_FILTER_NEAREST;
	samplerinfo.minFilter = VK_FILTER_NEAREST;
	samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VK_ASSERT(vkCreateSampler(Gfx::Device::GetInstance()->GetDevice(), &samplerinfo, nullptr, rt.GetSampler()), "failed to create sampler!");
}

void Gfx::Renderer::CreateSampler(RenderTarget* rt)
{
    VkSamplerCreateInfo samplerinfo{};
	samplerinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerinfo.pNext = nullptr;

	samplerinfo.magFilter = VK_FILTER_NEAREST;
	samplerinfo.minFilter = VK_FILTER_NEAREST;
	samplerinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    rt->SetSampler(true);
	VK_ASSERT(vkCreateSampler(Gfx::Device::GetInstance()->GetDevice(), &samplerinfo, nullptr, rt->GetSampler()), "failed to create sampler!");
}
