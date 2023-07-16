
#include "../includes/vkbuilder.h"

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

	if (enablevalidationlayers) {
		deletorhandler.pushfunction([=]() {
	        VkDebug::DestroyDebugUtilsMessengerEXT(instance, debugmessenger, nullptr);
		});
	}
	
	deletorhandler.pushfunction([=]() {
	    vkDestroyInstance(instance, nullptr);
	});
}

#ifdef OS_WINDOWS
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
#ifdef OS_LINUX
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
