#pragma once

#include <iostream>
#include <cstdio>
#include <stdexcept>

#include "anthraxAI/core/deletor.h"
#include "anthraxAI/utils/singleton.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_vulkan.h>

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

#define VK_ASSERT(x, s)                                         \
do                                                              \
{                                                               \
	VkResult err = x;                                           \
	std::string str = s;	                                    \
	if (err)                                                    \
	{        													\
		std::string vulkan = string_VkResult(err);              \
		std::string errstr = "Vulkan: Error: " + vulkan;		\
		errstr += "\n\n" + str;									\
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
		errstr += "\n\n";										\
		throw std::runtime_error(errstr);						\
	}                                                           \
} while (0)
