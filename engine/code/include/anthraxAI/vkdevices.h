#pragma once

#include "anthraxAI/vkdefines.h"
#include <optional>
#include <set>
#include <algorithm>

struct QueueBuilder {
	VkQueue graphicsqueue;
	VkQueue presentqueue;
};

struct QueueFamilyIndex {
	std::optional<uint32_t> graphicsfamily;
	std::optional<uint32_t> presentfamily;

	bool iscomplete() {
		return graphicsfamily.has_value()
		&& presentfamily.has_value();
	}
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR		capabilities;
    std::vector<VkSurfaceFormatKHR>	formats;
    std::vector<VkPresentModeKHR>	presentmodes;
};

class DeviceBuilder {
public:
#if defined(AAI_WINDOWS)
	void 						init(VkExtent2D windowextendh, HWND& hwndh, VkInstance& instanceh, VkSurfaceKHR& surfaceh, DeletionQueue*	deletor) 
								{ windowextend = windowextendh; hwnd = hwndh; instance = instanceh; surface = surfaceh; deletorhandler = deletor; };
#endif
#ifdef AAI_LINUX
	void 						init(VkExtent2D windowextendh, VkInstance& instanceh, VkSurfaceKHR& surfaceh, DeletionQueue*	deletor) 
								{ windowextend = windowextendh; instance = instanceh; surface = surfaceh; deletorhandler = deletor; };
#endif

	QueueFamilyIndex			findqueuefamilies(VkPhysicalDevice& device);
	SwapChainSupportDetails 	queryswapchainsupport(VkPhysicalDevice& device);
	bool						deviceextensionssupport(VkPhysicalDevice& device);
	bool						isdevisesuitable(VkPhysicalDevice device);

	void 						cleanswapchain();

	void 						findphysicaldevice();
	void 						buildlogicaldevice();
	void 						buildswapchain();
	void 						buildimagesview();
	void 						recreateswapchain(bool& winprepared, VkExtent2D windowextendh);

	VkExtent2D 					chooseswapextent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkPhysicalDevice&			getphysicaldevice()			{ return physicaldevbuilder;	};
	VkDevice&					getlogicaldevice()			{ return logicaldevbuilder;		};
	VkSwapchainKHR&				getswapchain()				{ return swapchainbuilder;		};
	VkFormat&					getswapchainformat()		{ return swapchainimgformatbuilder;};
	std::vector<VkImage>&		getswapchainimage()			{ return swapchainimgbuilder;	};
	std::vector<VkImageView>&	getswapchainimageview()		{ return swapchainimgviewsbuilder;};
	VkExtent2D&					getswapchainextent()		{ return swapchainextentbuilder;};
	QueueBuilder& 				getqueue()					{ return queue;					};
	VkSurfaceKHR&				getsurface()				{ return surface;				};
	VkExtent2D&					getwindowxtent()			{ return windowextend;			};

	size_t 						minuniformbufferoffsetalignment;

	AllocatedImage 				depthimage;
	AllocatedImage 				mainrendertarget;

private:
#if defined(AAI_WINDOWS)
	HWND 						hwnd;
#endif
	DeletionQueue*				deletorhandler;
	VkInstance					instance;
	VkSurfaceKHR 				surface;
	VkPhysicalDevice 			physicaldevbuilder;
	VkDevice					logicaldevbuilder;
	VkSwapchainKHR				swapchainbuilder;
	VkFormat 					swapchainimgformatbuilder;
	std::vector<VkImage>		swapchainimgbuilder;
	std::vector<VkImageView> 	swapchainimgviewsbuilder;
	VkExtent2D 					swapchainextentbuilder;
	VkExtent2D 					windowextend;

	QueueBuilder				queue;
};
