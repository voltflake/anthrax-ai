

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

#======================================= vulkan ======================================#

find_package(Vulkan REQUIRED)
list(APPEND _libraries Vulkan::Vulkan Vulkan::Headers)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(${Vulkan_LIBRARY_DIRS})
link_libraries(${Vulkan_LIBRARIES})
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")

#======================================== GLM ========================================#

set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
option(GLM_ENABLE_CXX_20 "" ON)
add_subdirectory(${_path}/glm)

list(APPEND _libraries glm::glm)

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
list(APPEND _libraries lib::imgui)

#======================================== stb ========================================#

make_library(stb)
list(APPEND _libraries lib::stb)

#=================================== tinyobjloader ===================================#

list(APPEND _includes ${_path}/tinyobjloader)

#=====================================================================================#


add_library(libraries INTERFACE)
add_library(anthrax::libraries ALIAS libraries)
target_link_libraries(libraries INTERFACE ${_libraries})
target_include_directories(libraries SYSTEM INTERFACE ${_includes})

if (WIN32)
	target_compile_definitions(libraries INTERFACE OS_WINDOWS)
elseif (UNIX)
	target_compile_definitions(libraries INTERFACE OS_LINUX)
endif()

unset(_libraries)
unset(_includes)
unset(_path)
