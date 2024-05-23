cmake_minimum_required(VERSION 3.25)

project(anthraxAI)

set(code_directory ${CMAKE_CURRENT_SOURCE_DIR}/code)
set(libs_directory ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(executable_name ${PROJECT_NAME}-demo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${libs_directory})

file(GLOB_RECURSE anthrax_sources CONFIGURE_DEPENDS ${code_directory}/src/*.cpp)
file(GLOB_RECURSE anthrax_headers CONFIGURE_DEPENDS ${code_directory}/include/*.h)

add_executable(${executable_name} ${anthrax_sources} ${anthrax_headers})
target_link_libraries(${executable_name} PUBLIC anthrax::libraries)
target_include_directories(${executable_name} PUBLIC ${code_directory}/include)

set_target_properties(${executable_name} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

set(assets_path ${CMAKE_SOURCE_DIR}/../assets)
include(${assets_path}/tools/compile_shaders.cmake)
compile_shaders(
	DIRECTORY ${assets_path}/shaders
)
