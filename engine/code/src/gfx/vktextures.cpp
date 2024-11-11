#include "anthraxAI/gfx/vkrenderer.h"
#include "anthraxAI/gfx/vkdevice.h"
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Gfx::Renderer::CreateTextures()
{
    std::string path = "./textures/";

    for (auto& it : Core::Scene::GetInstance()->GetResources()) {
        for (Core::ObjectInfo& info : it.second) {
            std::string texture = info.Texture;

            auto it = Textures.find(texture);
            if (it != Textures.end()) {
                continue;
            }
            Textures[texture] = CreateTexture(path + texture);
            CreateSampler(Textures[texture]);
        }
    }

	path = "./textures/dummy.png";
	Textures["dummy"] = CreateTexture(path);
	CreateSampler(Textures["dummy"]);
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
