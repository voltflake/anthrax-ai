#pragma once
#include <optional>
#include <vector>
#include <set>
#include <algorithm>

#include "anthraxAI/utils/mathdefines.h"
#include "anthraxAI/gfx/vkdefines.h"

namespace Gfx
{
    bool DeviceExtSupport(VkPhysicalDevice device) {
        uint32_t extensioncount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, nullptr);
        std::vector<VkExtensionProperties> availableextensions(extensioncount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, availableextensions.data());

        std::set<std::string> requiredextensions(DEVICE_EXT.begin(), DEVICE_EXT.end());
        for (const auto& extension : availableextensions) {
            requiredextensions.erase(extension.extensionName);
        }
        return requiredextensions.empty();
    }

    SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);
        uint32_t formatcount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatcount, nullptr);
        if (formatcount != 0) {
            details.Formats.resize(formatcount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatcount, details.Formats.data());
        }
        uint32_t presentmodecount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodecount, nullptr);
        if (presentmodecount != 0) {
            details.Presentmodes.resize(presentmodecount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodecount, details.Presentmodes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }
        return availableFormats[0];
    }

    VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablepresentmodes)
    {
        for (const auto& availablePresentMode : availablepresentmodes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Vector2<int>& extents)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
            return capabilities.currentExtent;
        else {
            int width, height;
            width = extents.x;
            height = extents.y;

            VkExtent2D extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return extent;
        }
    }
}
