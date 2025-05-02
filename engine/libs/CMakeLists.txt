include(FetchContent)

set(_libraries)
set(_includes)
set(_path ${CMAKE_CURRENT_SOURCE_DIR})

macro(make_library directory)
	if (NOT DEFINED ${directory}_sources)
		file(GLOB_RECURSE ${directory}_sources
			${_path}/${directory}/*.c**
			${_path}/${directory}/*.h**
		)
	endif()
	add_library(${directory} STATIC ${${directory}_sources})
	add_library(lib::${directory} ALIAS ${directory})
	target_include_directories(${directory} PUBLIC ${_path}/${directory})
endmacro(make_library)

#===================================== X11 / XCB =====================================#

if(aai_platform MATCHES "Linux")
	find_package(X11 REQUIRED)
	list(APPEND _libraries
		X11::X11
		X11::xcb
		X11::xcb_keysyms
		X11::xcb_xfixes
	)
endif()

#======================================= vulkan ======================================#

find_package(Vulkan REQUIRED)
list(APPEND _libraries Vulkan::Vulkan Vulkan::Headers)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(${Vulkan_LIBRARY_DIRS})
link_libraries(${Vulkan_LIBRARIES})
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")

#find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)
#======================================= imgui =======================================#

file(GLOB imgui_sources CONFIGURE_DEPENDS
	${_path}/imgui/*.cpp
	${_path}/imgui/*.h
	${_path}/imgui/misc/cpp/imgui_stdlib.cpp
	${_path}/imgui/misc/cpp/imgui_stdlib.h
)
if (WIN32)
	list(REMOVE_ITEM imgui_sources
		${_path}/imgui/imgui_impl_x11.cpp
		${_path}/imgui/imgui_impl_x11.h
	)
	list(APPEND imgui_sources
		${_path}/imgui/backends/imgui_impl_win32.cpp
		${_path}/imgui/backends/imgui_impl_win32.h
	)
endif()
make_library(imgui)
# imgui requires the Vulkan ans X11 libraries, so we need to link them here
target_link_libraries(imgui PRIVATE ${_libraries})
if (WIN32)
target_link_libraries(imgui PRIVATE dwmapi)
endif()
list(APPEND _libraries lib::imgui)

#======================================== GLM ========================================#

set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
option(GLM_ENABLE_CXX_20 "" ON)
add_subdirectory(${_path}/glm)

list(APPEND _libraries glm::glm)

#======================================== stb ========================================#

make_library(stb)
list(APPEND _libraries lib::stb)

#=================================== tinyobjloader ===================================#

list(APPEND _includes ${_path}/tinyobjloader)

#==================================Tracy Profiler=======================================#
FetchContent_Declare(
tracy
GIT_REPOSITORY https://github.com/wolfpld/tracy.git
GIT_TAG master
GIT_SHALLOW TRUE
GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy)
#=================================== assimp ===================================#

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "" FORCE)
configure_file("${PROJECT_SOURCE_DIR}/libs/assimp/include/assimp/config.h.in" "${PROJECT_SOURCE_DIR}/libs/assimp/include/assimp/config.h")
configure_file("${PROJECT_SOURCE_DIR}/libs/assimp/contrib/zlib/zconf.h.cmakein" "libs/assimp/contrib/zlib/zconf.h")
add_subdirectory(${_path}/assimp)
list(APPEND _libraries assimp::assimp)
#target_include_directories(PUBLIC ${_path}/assimp/include)

#list(APPEND _libraries assimp::assimp)

# add_subdirectory(${_path}/assimp)

#=====================================================================================#
#=================================== openAL-soft ===================================#

if (WIN32)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(LIBTYPE STATIC)

add_subdirectory(${_path}/openal-soft/openal-soft)

list(APPEND _includes ${_path}/openal-soft/openal-soft/include)

list(APPEND _libraries OpenAL::OpenAL)
elseif(UNIX)

list(APPEND _includes ${_path}/openal-soft/openal-soft/include)

add_subdirectory(${_path}/openal-soft/openal-soft)
list(APPEND _libraries OpenAL::OpenAL)
endif()
#=====================================================================================#

list(REMOVE_DUPLICATES _libraries)
list(REMOVE_DUPLICATES _includes)

add_library(libraries INTERFACE)
add_library(anthrax::libraries ALIAS libraries)
target_link_libraries(libraries INTERFACE ${_libraries})

if (WIN32)
	target_link_directories(libraries INTERFACE "$ENV{VULKAN_SDK}/Lib/")
endif()

target_link_libraries(libraries INTERFACE shaderc_shared )

target_link_libraries(libraries INTERFACE Tracy::TracyClient )
target_include_directories(libraries SYSTEM INTERFACE ${_includes})

if (WIN32)
	target_compile_definitions(libraries INTERFACE OS_WINDOWS)
elseif (UNIX)
	target_compile_definitions(libraries INTERFACE OS_LINUX)
endif()

unset(_libraries)
unset(_includes)
unset(_path)
