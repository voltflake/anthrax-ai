
#include "anthraxAI/vkbuilder.h"

void VkBuilder::buildinstance() {
	
	VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "MyAppName";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "MyEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (!instanceextensions.empty()) {
		ASSERT(!instanceextensionssupport(), "Not supported required instance extensions!");

		createInfo.ppEnabledExtensionNames = instanceextensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceextensions.size());
	}

	ASSERT((enablevalidationlayers && !validationlayerssupport()), "Not supported validation layers!");

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enablevalidationlayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationlayer.size());
		createInfo.ppEnabledLayerNames = validationlayer.data();

        VkDebug::populateDbgMsgCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
	}

	VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &(instance)), "vkCreateInstance failed");

	VkDebug::setupDebugMessenger(instance, debugmessenger, enablevalidationlayers);

	deletorhandler.pushfunction([=]() {
	    vkDestroyInstance(instance, nullptr);
	});

	if (enablevalidationlayers) {
		deletorhandler.pushfunction([=]() {
	        VkDebug::DestroyDebugUtilsMessengerEXT(instance, debugmessenger, nullptr);
		});
	}
	
}

#if defined(AAI_WINDOWS)
void VkBuilder::buildwinsurface(HWND& hwnd, HINSTANCE& hinstance)
{
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = hwnd;
	createInfo.hinstance = hinstance;

	VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface), "failed to create window surface!");

	deletorhandler.pushfunction([=]() {
	    vkDestroySurfaceKHR(instance, surface, nullptr);
	});
}
#endif
#ifdef AAI_LINUX
void VkBuilder::buildlinuxsurface(xcb_connection_t* connection, xcb_window_t& window)
{
	VkXcbSurfaceCreateInfoKHR surfaceinfo = {};
	surfaceinfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	surfaceinfo.pNext = NULL;
	surfaceinfo.flags = 0;
	surfaceinfo.connection = connection;
	surfaceinfo.window = window;

	VK_ASSERT(vkCreateXcbSurfaceKHR(instance, &surfaceinfo, NULL, &surface), "failed to create window surface!");

	deletorhandler.pushfunction([=]() {
	    vkDestroySurfaceKHR(instance, surface, nullptr);
	});
}
#endif

void VkBuilder::resizewindow(bool& winprepared, VkExtent2D windowextendh, bool check) {

	devicehandler.recreateswapchain(winprepared, windowextendh);
	builddepthbuffer();
	buildmainimage();
	renderer.recreateframebuffer(devicehandler);
	pipeline.recreatepipeline(getdevice(), check);

	descriptors.updateattachmentdescriptors(devicehandler);

}

bool VkBuilder::instanceextensionssupport()
{
	unsigned int instextcount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &instextcount, nullptr);
	
	std::vector<VkExtensionProperties> availableextensions(instextcount);

	vkEnumerateInstanceExtensionProperties(nullptr, &instextcount, availableextensions.data());

	for (const char *requiredextname : instanceextensions) {
		bool found = false;
		for (const VkExtensionProperties &extproperties : availableextensions) {
			if (strcmp(requiredextname, extproperties.extensionName) == 0) {
				found = true;
				break ;
			}
		}
		if (!found) {
			return false;
		}
	}

	return true;
}

bool VkBuilder::validationlayerssupport() {
	
	uint32_t layercount;
	vkEnumerateInstanceLayerProperties(&layercount, nullptr);

	std::vector<VkLayerProperties> availablelayers(layercount);

	vkEnumerateInstanceLayerProperties(&layercount, availablelayers.data());

	for (const char* layername : validationlayer) {
	    bool found = false;

	    for (const auto& layerproperties : availablelayers) {
	        if (strcmp(layername, layerproperties.layerName) == 0) {
	            found = true;
	            break;
	        }
	    }

	    if (!found) {
	        return false;
	    }
	}
	return true;
}

void VkBuilder::copycheck(uint32_t swapchainimageindex) {

		renderer.submit([&](VkCommandBuffer cmd) {
		VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texturehandler.gettexture("check/back.jpg")->image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
  		 barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
       

        vkCmdPipelineBarrier(
            cmd,
             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

	});
		renderer.submit([&](VkCommandBuffer cmd) {
		VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = getswapchainimage()[swapchainimageindex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
		barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  	 	barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
       

        vkCmdPipelineBarrier(
            cmd,
             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

	});
			
	renderer.submit([&](VkCommandBuffer cmd){

	VkOffset3D blitSize;
	blitSize.x = getswapchainextent().width;
	blitSize.y = getswapchainextent().height;
	blitSize.z = 1;
	VkImageBlit imageBlitRegion{};
	imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.srcSubresource.layerCount = 1;
	imageBlitRegion.srcOffsets[1] = blitSize;
	imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.dstSubresource.layerCount = 1;
	imageBlitRegion.dstOffsets[1] = blitSize;

	vkCmdBlitImage(
		cmd,
		getswapchainimage()[swapchainimageindex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		texturehandler.gettexture("check/back.jpg")->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageBlitRegion,
		VK_FILTER_NEAREST);
	});

	renderer.submit([&](VkCommandBuffer cmd) {
	VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout =  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texturehandler.gettexture("check/back.jpg")->image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;//VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
	});

	renderer.submit([&](VkCommandBuffer cmd) {
	VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = getswapchainimage()[swapchainimageindex];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
   

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

	});
}

