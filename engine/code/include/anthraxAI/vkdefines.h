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

#include <assimp/Importer.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
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

#define MAX_FPS 120
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
#else
#define VK_USE_PLATFORM_WIN32_KHR
	#define MOUSE_LEFT 1
	#define ESC_KEY 0x1B
	#define W_KEY 0x57
	#define D_KEY 0x44
	#define A_KEY 0x41
	#define S_KEY 0x53
	#define ENTER_KEY 0x0D
	#define PLUS_KEY  0xBB
	#define MINUS_KEY 0xBD
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

static inline glm::mat4 mat2glm(aiMatrix4x4 from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

static inline glm::mat3 mat2glm(aiMatrix3x3 from)
{
	glm::mat3 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3;
	return to;
}

static inline glm::quat quat2glm(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}

static inline glm::vec3 vec2glm(aiVector3D vec) 
{
	glm::vec3 v;
	v.x = vec.x; 
	v.y = vec.y;
	v.z = vec.z;
	return v;
}

enum EngineState {
	INIT_ENGINE 	= 1 << 0,	/* 0000 0001 */
	ENGINE_EDITOR 	= 1 << 1, 	/* 0000 0010 */
	HANDLE_EVENT 	= 1 << 2,	/* 0000 0100 */
	NEW_LEVEL 		= 1 << 3,	/* 0000 1000 */
	LOAD_LEVEL 		= 1 << 4,	/* 0001 0000 */
	PLAY_GAME 		= 1 << 5,	/* 0010 0000 */
	PAUSE_GAME  	= 1 << 6,	/* 0100 0000 */
	EXIT  			= 1 << 7,	/* 1000 0000 */
	MODE_2D  		= 1 << 8,	/* 1000 0000 0 */
	MODE_3D  		= 1 << 9,	/* 1000 0000 00*/
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
	VkFormat format;
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

#define BONE_INFLUENCE 4
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
	glm::vec2 uv;

	int boneID[4];
	float weights[4];
};

struct CameraData {

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 viewpos;
	glm::vec4 mousepos;
	glm::vec4 viewport;

	glm::vec4 lightcolor = glm::vec4(0.63f, 0.82f, 0.48f, 1);
	glm::vec4 lightpos= glm::vec4(1.2f, 1.0f, 2.0f, 1);
	float ambient = 0.1;
	float diffuse = 0.5;
	float specular = 0.5;
};

#define DEPTH_ARRAY_SCALE 512
#define MAX_BONES 200
struct StorageData {
    glm::mat4 bonesmatrices[MAX_BONES];
	u_int data[DEPTH_ARRAY_SCALE] = {0};
};

#define BONE_ARRAY_SIZE (sizeof(glm::mat4) * MAX_BONES)

struct AnimationTransforms {
    glm::mat4* bonesmatrices{nullptr};
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

	Positions() {};
	Positions(int tmpx, int tmpy) { x = tmpx; y = tmpy; };
};

struct Positions3 {
	float x;
	float y;
	float z;

	Positions3() {};
	Positions3(const Positions3& tmp) { x = tmp.x; y = tmp.y; z = tmp.z; };
	Positions3(int tmpx, int tmpy, int tmpz) { x = tmpx; y = tmpy; z = tmpz; };
};

#define GIZMO_HEIGHT 3
struct Gizmo {
	Positions3 		pos = {0, 0, 0};
	bool 			visible = false;
	int 			objecthandler = 0;
	GizmoAxis 		axis = AXIS_UNDEF;
};

struct Data {
	std::string texturepath;
	Positions3 pos;
	bool debugcollision;
	bool animation;
	bool material;

	Data() {}
	Data(const std::string& path, Positions3 p, bool debugcol, bool anim, bool mat = false)
	: texturepath(path), pos(p), debugcollision(debugcol), animation(anim), material(mat) {}
};

const int MAX_FRAMES_IN_FLIGHT = 2;
typedef std::array<FrameData, MAX_FRAMES_IN_FLIGHT> FrameArray;
typedef std::array<BufferHandler, MAX_FRAMES_IN_FLIGHT> UboArray;
typedef std::array<BufferHandler, MAX_FRAMES_IN_FLIGHT> StorageArray;
typedef std::array<BufferHandler, MAX_FRAMES_IN_FLIGHT> AnimationTransformsArray;
typedef std::unordered_map<int, Data> ResourcesMap;


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
