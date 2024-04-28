#pragma once

#define OS_LINUX
// #define OS_WINDOWS
#include "vulkan/vulkan.h"

#ifdef OS_LINUX
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_keysyms.h>
#include "vulkan/vulkan_xcb.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_x11.h"
#include "imgui/imgui_impl_vulkan.h"
static ImGui_ImplVulkanH_Window MainWindowData;

static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t *conn, bool only_if_exists, const char *str)
{
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, only_if_exists, strlen(str), str);
	return xcb_intern_atom_reply(conn, cookie, NULL);
}

#endif
#ifdef OS_WINDOWS
#include <windows.h>
#include "vulkan/vulkan_win32.h"
#endif

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <thread>
#include <vector>
#include <array>
#include <sstream>
#include <cmath>
#include <stdio.h>
#include <unordered_map>
#include <cstring>
#include <functional>
#include <deque>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "vulkan/vk_enum_string_helper.h"

#include "lookup.h"

#define VK_ASSERT(x, s)                                         \
do                                                              \
{                                                               \
	VkResult err = x;                                           \
	std::string str = s;	                                    \
	if (err)                                                    \
	{        													\
		std::string vulkan = string_VkResult(err);              \
		std::string errstr = "Vulkan: Error: " + vulkan;		\
		errstr += "\n" + str;									\
		throw std::runtime_error(errstr);						\
	}                                                           \
} while (0)

#define ASSERT(x, s)	                                        \
do                                                              \
{                                                               \
	bool err = x;												\
	std::string str = s;	                                    \
	if (err)                                                   	\
	{                                                           \
		std::string errstr = "Error: " + str;					\
		throw std::runtime_error(errstr);						\
	}                                                           \
} while (0)

#ifdef OS_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifdef OS_LINUX
#define VK_USE_PLATFORM_XCB_KHR
#define ESC_KEY 65307
#define W_KEY 119
#define D_KEY 100
#define A_KEY 97
#define S_KEY 115
#define ENTER_KEY 65293
#endif

#define _USE_MATH_DEFINE

#define FRAG_SHADER "./shaders/simpleShader.frag.spv"
#define VERTEX_SHADER "./shaders/simpleShader.vert.spv"

struct DeletionQueue {
	std::deque<std::function<void()>> deletors;

	void pushfunction(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void cleanall() {
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)();
		}
		deletors.clear();
	}
};

struct BufferHandler {
	VkBuffer buffer;
	VkDeviceMemory devicememory;

	void* uniformmapedmemory;
};

struct VertexInputDescription {

	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
	glm::vec2 uv;
    //static VertexInputDescription getvertexdescription();
};

struct CameraData {
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec2 pos;
	glm::vec2 viewport;
};

struct FrameData {
	VkSemaphore PresentSemaphore, RenderSemaphore;
	VkFence RenderFence;	

	VkCommandPool CommandPool;
	VkCommandBuffer MainCommandBuffer;
};

struct UploadContext {
	VkFence UploadFence;
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
};

struct Positions {
	int x;
	int y;
};

struct Data {
	std::string texturepath;
	Positions pos;
	bool debugcollision;
	bool animation;
};

const int MAX_FRAMES_IN_FLIGHT = 2;
typedef std::array<FrameData, MAX_FRAMES_IN_FLIGHT> FrameArray;
typedef std::array<BufferHandler, MAX_FRAMES_IN_FLIGHT> UboArray;

const std::vector<const char *> validationlayer = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceextenstions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef OS_WINDOWS
const std::vector<const char*> instanceextensions = 
{VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#endif
#ifdef OS_LINUX
const std::vector<const char*> instanceextensions = 
{VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_xcb_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#endif

#ifdef NDEBUG
    const bool enablevalidationlayers = false;
#else
    const bool enablevalidationlayers = true;
#endif
