cmake_minimum_required(VERSION 3.25)

project(anthraxAI
	VERSION 0.1.0
	DESCRIPTION "Anthrax AI | The Vulkan Based Game Engine"
	LANGUAGES CXX
)

set(aai_engine_code_dir ${aai_engine_dir}/code)
set(aai_engine_libs_dir ${aai_engine_dir}/libs)

add_subdirectory(${aai_engine_libs_dir})

file(GLOB_RECURSE anthrax_sources CONFIGURE_DEPENDS ${aai_engine_code_dir}/src/*.cpp)
file(GLOB_RECURSE anthrax_headers CONFIGURE_DEPENDS ${aai_engine_code_dir}/include/*.h)

add_library(${PROJECT_NAME} ${anthrax_sources} ${anthrax_headers})
add_library(anthraxAI::engine ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC anthrax::libraries)
target_include_directories(${PROJECT_NAME} PUBLIC ${aai_engine_code_dir}/include)
