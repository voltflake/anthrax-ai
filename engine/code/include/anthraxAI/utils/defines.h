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
