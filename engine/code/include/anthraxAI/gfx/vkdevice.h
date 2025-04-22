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

    enum QueuesEnum {
        GRAPHICS_QUEUE = 0,
        PRESENT_QUEUE
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

            void RecreateSwapchain();
            VkDevice GetDevice() const { return LogicalDevice; }
            VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
            VkSurfaceKHR GetSurface() const { return Surface; }

            QueueFamilyIndex FindQueueFamilies(VkPhysicalDevice device);
            bool IsDeviceSuitable(VkPhysicalDevice device);

            VkQueue GetQueue(QueuesEnum q);

            VkFormat* GetSwapchainFormat() { return &Swapchain.Format; }
            VkSwapchainKHR& GetSwapchain() { return Swapchain.Swapchain; }
            VkExtent2D GetSwapchainExtent() const { return Swapchain.Extent; }
            Vector2<int> GetSwapchainSize() const { return { static_cast<int>(Swapchain.Extent.width), static_cast<int>(Swapchain.Extent.height) }; }
            VkImageView GetSwapchainImageView(uint32_t ind) { return Swapchain.ImageViews[ind]; }
            VkImage GetSwapchainImage(uint32_t ind) {  return Swapchain.Images[ind]; }

            void CleanUpSwapchain();

	        size_t MinUniformBufferOffsetAlignment;
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
    };
}
