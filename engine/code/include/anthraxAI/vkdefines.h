#pragma once

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
#include <algorithm>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(AAI_LINUX)
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_keysyms.h>
#include <vulkan/vulkan_xcb.h>
#include <imgui_impl_x11.h>
static ImGui_ImplVulkanH_Window MainWindowData;

static inline xcb_intern_atom_reply_t* intern_atom_helper(xcb_connection_t *conn, bool only_if_exists, const char *str)
{
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, only_if_exists, strlen(str), str);
	return xcb_intern_atom_reply(conn, cookie, NULL);
}

#elif defined(AAI_WINDOWS)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include <backends/imgui_impl_win32.h>
#endif

#include "anthraxAI/lookup.h"

#define MAX_FPS 60
#define FPS_SAMPLER 100

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

#if defined(AAI_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifdef AAI_LINUX
#define VK_USE_PLATFORM_XCB_KHR
#define ESC_KEY 65307
#define W_KEY 119
#define D_KEY 100
#define A_KEY 97
#define S_KEY 115
#define ENTER_KEY 65293
#define PLUS_KEY  61
#define MINUS_KEY 45
#endif

#define _USE_MATH_DEFINE

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

struct Texture {
	VkImage image;
	VkImageView imageview;
	VkDeviceMemory memory;

	VkSampler sampler;
	float w;
	float h;
};

struct AllocatedImage {
    Texture* texture = nullptr;
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
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 viewpos;
	glm::vec4 pos;
	glm::vec4 viewport;

	glm::vec4 lightcolor = glm::vec4(0.63f, 0.82f, 0.48f, 1);
	glm::vec4 lightpos= glm::vec4(1.2f, 1.0f, 2.0f, 1);
	float ambient = 0.1;
	float diffuse = 0.5;
	float specular = 0.5;
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

struct DebugAnim {
	int scale = 1;
	VkDescriptorSet desc;
	std::string path;
};

struct Positions {
	int x;
	int y;

	Positions() {};
	Positions(int tmpx, int tmpy) { x = tmpx; y = tmpy; };
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
#if defined(AAI_WINDOWS)
const std::vector<const char*> instanceextensions = 
{VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#endif
#ifdef AAI_LINUX
const std::vector<const char*> instanceextensions = 
{VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_xcb_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#endif

#ifdef NDEBUG
    const bool enablevalidationlayers = false;
#else
    const bool enablevalidationlayers = true;
#endif
