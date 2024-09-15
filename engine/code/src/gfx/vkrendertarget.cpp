#include "anthraxAI/gfx/vkrendertarget.h"
#include "anthraxAI/gfx/vkdevice.h"

Gfx::RenderTarget::RenderTarget(const RenderTarget& rt) :
    Format(rt.Format), Dimensions(rt.Dimensions),
    IsDepth(rt.IsDepth), IsStorage(rt.IsStorage)
{
}

VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
    VkImageCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspectFlags;

	return info;
}

void Gfx::RenderTarget::CreateRenderTarget()
{
    VkImageUsageFlags usageflags{};
    if (IsDepth) {
        usageflags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }
    else {
        usageflags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }
    if (IsStorage) {
	    usageflags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    VkImageCreateInfo imginfo = ImageCreateInfo(Format, usageflags, { static_cast<uint32_t>(Dimensions.x), static_cast<uint32_t>(Dimensions.y), 1 });

    VK_ASSERT(vkCreateImage(Gfx::Device::GetInstance()->GetDevice(), &imginfo, nullptr, &Image), "failed to create image");
}