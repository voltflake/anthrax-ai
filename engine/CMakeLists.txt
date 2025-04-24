cmake_minimum_required(VERSION 3.25)

project(anthraxAI
    VERSION 0.1.0
    DESCRIPTION "Anthrax AI | The Vulkan Based Game Engine"
    LANGUAGES CXX
)

file(GLOB_RECURSE anthrax_sources CONFIGURE_DEPENDS "code/src/*.cpp")
add_executable(anthraxAI ${anthrax_sources})
target_include_directories(anthraxAI PRIVATE "code/include")

# Find Vulkan and include directories
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    target_include_directories(anthraxAI PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(anthraxAI PRIVATE ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")
else()
    message(FATAL_ERROR "Vulkan not found. Please ensure Vulkan SDK is installed and available.")
endif()

# Find GLM and include directories
set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
option(GLM_ENABLE_CXX_20 "" ON)
add_subdirectory(libs/glm)
target_include_directories(anthraxAI PRIVATE libs/glm)
target_link_libraries(anthraxAI PRIVATE glm::glm)

# Add x11 and XCB libraries for Linux
if(UNIX)
	find_package(X11 REQUIRED)
    target_link_libraries(anthraxAI
    PRIVATE X11::X11
    PRIVATE X11::xcb
    PRIVATE X11::xcb_keysyms
    PRIVATE X11::xcb_xfixes
    )
endif()

# Create ImGui library
file(GLOB IMGUI_SOURCES
    libs/imgui/*.cpp
    libs/imgui/misc/cpp/imgui_stdlib.cpp
    libs/imgui/backends/imgui_impl_vulkan.cpp
)
file(GLOB IMGUI_HEADERS
    libs/imgui/*.h
    $libs/imgui/misc/cpp/imgui_stdlib.h
    libs/imgui/imgui_impl_vulkan.h
)
if (WIN32)
    list(APPEND IMGUI_SOURCES libs/imgui/backends/imgui_impl_win32.cpp)
    list(APPEND IMGUI_HEADERS libs/imgui/backends/imgui_impl_win32.h)
endif()
if(UNIX)
    list(APPEND IMGUI_SOURCES libs/imgui/imgui_impl_x11.cpp)
    list(APPEND IMGUI_HEADERS libs/imgui/imgui_impl_x11.h)
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE libs/imgui)
target_include_directories(imgui PRIVATE libs/imgui/backends)
target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(imgui PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(imgui PRIVATE ${XCB_LIBRARIES} PRIVATE X11::X11)
if (WIN32)
    target_link_libraries(imgui PRIVATE dwmapi)
endif()
target_link_libraries(anthraxAI PRIVATE imgui)
target_include_directories(anthraxAI PRIVATE libs/imgui)
target_include_directories(anthraxAI PRIVATE libs/imgui/backends)

# assimp
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "" FORCE)
configure_file("libs/assimp/include/assimp/config.h.in" "libs/assimp/include/assimp/config.h")
configure_file("libs/assimp/contrib/zlib/zconf.h.cmakein" "libs/assimp/contrib/zlib/zconf.h")
add_subdirectory(libs/assimp)

target_link_libraries(anthraxAI PRIVATE assimp::assimp)
target_include_directories(anthraxAI PRIVATE libs/assimp/include)

# openal
add_subdirectory(libs/openal-soft)
if(WIN32)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
    add_definitions(-DAL_LIBTYPE_STATIC)
endif()
target_link_libraries(anthraxAI PRIVATE OpenAL::OpenAL)
target_include_directories(anthraxAI PRIVATE libs/openal-soft/include)

# stb
file(GLOB STB_HEADERS libs/stb/*.h)
file(GLOB STB_SOURCES libs/stb/*.c)
add_library(stb ${STB_SOURCES})
target_link_libraries(anthraxAI PRIVATE stb)
target_include_directories(anthraxAI PRIVATE libs/stb)

# shaderc
target_link_libraries(anthraxAI PUBLIC shaderc_shared)