cmake_minimum_required(VERSION 3.25)

set(aai_root ${CMAKE_CURRENT_SOURCE_DIR}) # aai - Anthrax AI

project(aai)

include(${aai_root}/cmake/options.cmake)

add_subdirectory(${aai_engine_dir})

if(AAI_BUILD_DEMOS)
	add_subdirectory(${aai_demos_dir})
endif()

if(AAI_COMPILE_SHADERS)
	include(AAICompileShaders)
	aai_compile_shaders(DIRECTORY ${aai_assets_dir}/shaders)
endif()
