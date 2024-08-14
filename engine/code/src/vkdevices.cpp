
#include "anthraxAI/vkdevices.h"

void DeviceBuilder::findphysicaldevice() {
	uint32_t devicecount = 0;
	vkEnumeratePhysicalDevices(instance, &devicecount, nullptr);

	ASSERT((devicecount == 0), "failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(devicecount);
	vkEnumeratePhysicalDevices(instance, &devicecount, devices.data());

	for (const auto &devs : devices) {
		if (isdevisesuitable(devs)) {
			physicaldevbuilder = devs;
			break;
		}
	}

	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(physicaldevbuilder, &props);
	std::cout << "\nDevice: " << props.deviceName << '\n';
	std::cout << "The GPU has a minimum buffer alignment of " << props.limits.minUniformBufferOffsetAlignment << std::endl;	
	minuniformbufferoffsetalignment = props.limits.minUniformBufferOffsetAlignment;
	ASSERT(physicaldevbuilder == VK_NULL_HANDLE, "failed to find a suitable GPU");
}

void DeviceBuilder::buildlogicaldevice() {
 	
 	QueueFamilyIndex indices = findqueuefamilies(physicaldevbuilder);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsfamily.value(), indices.presentfamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceextenstions.size());
    createInfo.ppEnabledExtensionNames = deviceextenstions.data();

    if (enablevalidationlayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationlayer.size());
        createInfo.ppEnabledLayerNames = validationlayer.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

	VK_ASSERT(vkCreateDevice(physicaldevbuilder, &createInfo, nullptr, &logicaldevbuilder), "failed to create logical device!");

	vkGetDeviceQueue(logicaldevbuilder, indices.graphicsfamily.value(), 0, &queue.graphicsqueue);
	vkGetDeviceQueue(logicaldevbuilder, indices.presentfamily.value(), 0, &queue.presentqueue);

	deletorhandler->pushfunction([=]() {
	    vkDestroyDevice(logicaldevbuilder, nullptr);
	});
}

QueueFamilyIndex DeviceBuilder::findqueuefamilies(VkPhysicalDevice& device) {

	QueueFamilyIndex index;

	uint32_t queuefamilycount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilycount, nullptr);

	std::vector<VkQueueFamilyProperties> queuefamilies(queuefamilycount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilycount, queuefamilies.data());

	int ind = 0;
	for (const auto& queuefam : queuefamilies) {
		if (index.iscomplete()) {
			break ;
		}
		if (queuefam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			index.graphicsfamily = ind;
		}
		
		VkBool32 presentsupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, ind, surface, &presentsupport);
		if (presentsupport) {
			index.presentfamily = ind;
		}
		ind++;
	}

	return index;
}

bool DeviceBuilder::deviceextensionssupport(VkPhysicalDevice& device) {
    
    uint32_t extensioncount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, nullptr);

    std::vector<VkExtensionProperties> availablexxtensions(extensioncount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensioncount, availablexxtensions.data());

    std::set<std::string> requiredextensions(deviceextenstions.begin(), deviceextenstions.end());

    for (const auto& extension : availablexxtensions) {
        requiredextensions.erase(extension.extensionName);
    }

    return requiredextensions.empty();
}


bool DeviceBuilder::isdevisesuitable(VkPhysicalDevice device) {
	
	QueueFamilyIndex index;

	index = findqueuefamilies(device);
	bool extensionsupported = deviceextensionssupport(device);

	bool swapchainsupport = false;

	if (extensionsupported) {
		SwapChainSupportDetails details = queryswapchainsupport(device);
		swapchainsupport = !details.formats.empty() && !details.presentmodes.empty();
	}

	return index.iscomplete() && extensionsupported && swapchainsupport;
}

SwapChainSupportDetails DeviceBuilder::queryswapchainsupport(VkPhysicalDevice& device) 
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatcount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatcount, nullptr);

	if (formatcount != 0) {
	    details.formats.resize(formatcount);
	    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatcount, details.formats.data());
	}

	uint32_t presentmodecount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodecount, nullptr);

	if (presentmodecount != 0) {
	    details.presentmodes.resize(presentmodecount);
	    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentmodecount, details.presentmodes.data());
	}

    return details;
}

VkSurfaceFormatKHR chooseswapsurfaceformat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
	for (const auto& availableFormat : availableFormats) 
	{
	    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
	    && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	        return availableFormat;
	}
	return availableFormats[0];
}

VkPresentModeKHR chooseswappresentmode(const std::vector<VkPresentModeKHR>& availablepresentmodes)
{
	for (const auto& availablePresentMode : availablepresentmodes) 
	{
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D DeviceBuilder::chooseswapextent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
   	if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;
    else 
    {
        int width, height;

#if defined(AAI_WINDOWS)
    	RECT rect;
		if(GetWindowRect(hwnd, &rect))
		{
		  width = rect.right - rect.left;
		  height = rect.bottom - rect.top;
		}
#endif
#ifdef AAI_LINUX
		width = windowextend.width;
		height = windowextend.height;
#endif
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void DeviceBuilder::buildswapchain() {
   
	SwapChainSupportDetails swapchainsupport = queryswapchainsupport(physicaldevbuilder);

    VkSurfaceFormatKHR surfaceFormat = chooseswapsurfaceformat(swapchainsupport.formats);
    VkPresentModeKHR presentMode = chooseswappresentmode(swapchainsupport.presentmodes);
    VkExtent2D extent = chooseswapextent(swapchainsupport.capabilities);

    uint32_t imagecount = swapchainsupport.capabilities.minImageCount + 1;

    if (swapchainsupport.capabilities.maxImageCount > 0 && imagecount > swapchainsupport.capabilities.maxImageCount)
    	imagecount = swapchainsupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imagecount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	QueueFamilyIndex indices = findqueuefamilies(physicaldevbuilder);
	uint32_t queueFamilyIndices[] = {indices.graphicsfamily.value(), indices.presentfamily.value()};

	if (indices.graphicsfamily != indices.presentfamily) {
	    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	    createInfo.queueFamilyIndexCount = 2;
	    createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} 
	else {
	    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	    createInfo.queueFamilyIndexCount = 0; // Optional
	    createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapchainsupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_ASSERT(vkCreateSwapchainKHR(logicaldevbuilder, &createInfo, nullptr, &swapchainbuilder), "failed to create swap chain!");

	vkGetSwapchainImagesKHR(logicaldevbuilder, swapchainbuilder, &imagecount, nullptr);
	swapchainimgbuilder.resize(imagecount);
	vkGetSwapchainImagesKHR(logicaldevbuilder, swapchainbuilder, &imagecount, swapchainimgbuilder.data());
	
	swapchainimgformatbuilder = surfaceFormat.format;
	swapchainextentbuilder = extent;
}

void DeviceBuilder::buildimagesview() {

	swapchainimgviewsbuilder.resize(swapchainimgbuilder.size());

	for (size_t i = 0; i < swapchainimgbuilder.size(); i++) 
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainimgbuilder[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainimgformatbuilder;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VK_ASSERT(vkCreateImageView(logicaldevbuilder, &createInfo, nullptr, &swapchainimgviewsbuilder[i]), "failed to create image view!");

	}
}

void DeviceBuilder::cleanswapchain() {
	vkDestroySwapchainKHR(logicaldevbuilder, swapchainbuilder, nullptr);

	for (size_t i = 0; i < swapchainimgbuilder.size(); i++) {
		vkDestroyImageView(logicaldevbuilder, swapchainimgviewsbuilder[i], nullptr);
	}
}

void DeviceBuilder::recreateswapchain(bool& winprepared, VkExtent2D windowextendh) {

	if (!winprepared) {
		return;
	}
	winprepared = false;

	windowextend = windowextendh;

	vkDeviceWaitIdle(logicaldevbuilder);

	cleanswapchain();
	
	buildswapchain();
	buildimagesview();

	std::cout << "recreated swapchain\n";
}
