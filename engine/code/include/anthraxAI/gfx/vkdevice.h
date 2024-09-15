#pragma once
#include "anthraxAI/utils/defines.h"
#include "anthraxAI/gfx/vkbase.h"
#include "anthraxAI/core/windowmanager.h"
#include <optional>

namespace Gfx
{
    struct QueueFamilyIndex {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Present;

        bool IsComplete() {
            return Graphics.has_value()
            && Present.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR		Capabilities;
        std::vector<VkSurfaceFormatKHR>	Formats;
        std::vector<VkPresentModeKHR>	Presentmodes;
    };

    struct Queues {
        VkQueue Graphics;
        VkQueue Present;
    };

    struct SwapchainInfo {
        VkSwapchainKHR				Swapchain;
	    VkFormat 					Format;
	    std::vector<VkImage>		Images;
	    std::vector<VkImageView> 	ImageViews;
	    VkExtent2D 			    	Extent;
    };

    class Device : public Utils::Singleton<Device>
    {
        public:
            void Init();

            VkDevice GetDevice() const { return LogicalDevice; }
            VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
            VkSurfaceKHR GetSurface() const { return Surface; }

            QueueFamilyIndex FindQueueFimilies(VkPhysicalDevice device);
            bool IsDeviceSuitable(VkPhysicalDevice device);

        private:
#ifdef AAI_LINUX
            void CreateLinuxSurface();
#else
            void CreateWindowsSurface();
#endif
            void CreatePhysicalDevice();
            void CreateDevice();

            void CreateSwapchain();
            void CreateSwapchainImageViews();

            VkDevice LogicalDevice;
            VkPhysicalDevice PhysicalDevice;
        	VkSurfaceKHR Surface;
            SwapchainInfo Swapchain;

            Queues Queue;

	        size_t MinUniformBufferOffsetAlignment;
    };
}