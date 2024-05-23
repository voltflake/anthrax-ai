macro(msg)
	if(NOT CS_SILENT)
		message(${ARGN})
	endif()
endmacro()

function(compile_shaders)
	include(CMakeParseArguments)

	cmake_parse_arguments(CS "SILENT" "DIRECTORY;OUTPUT_DIRECTORY" "EXTENSIONS" ${ARGN})

	if(NOT CS_DIRECTORY)
		message(FATAL_ERROR "compile_shaders: DIRECTORY not specified")
	endif()
	cmake_path(ABSOLUTE_PATH CS_DIRECTORY NORMALIZE)

	if(NOT CS_OUTPUT_DIRECTORY)
		msg(STATUS "compile_shaders: OUTPUT_DIRECTORY not specified, using ${CS_DIRECTORY}")
		set(CS_OUTPUT_DIRECTORY ${CS_DIRECTORY})
	endif()

	find_package(Vulkan REQUIRED QUIET COMPONENTS glslc)
	if (NOT Vulkan_glslc_FOUND)
		message(FATAL_ERROR "compile_shaders: Vulkan glslc not found")
	endif()
	msg(STATUS "compile_shaders: Found Vulkan glslc at ${Vulkan_GLSLC_EXECUTABLE}")

	set(_extensions ".vert" ".frag" ".geom" ".tesc" ".tese" ".comp")
	list(APPEND _extension ${CS_EXTENSIONS})

	set(_shaders)
	foreach(extension IN LISTS _extensions)
		file(GLOB_RECURSE tmp CONFIGURE_DEPENDS ${CS_DIRECTORY}/*${extension})
		list(APPEND _shaders ${tmp})
	endforeach()

	list(LENGTH _shaders _shaders_count)
	msg(STATUS "compile_shaders: Compiling ${_shaders_count} shaders from ${CS_DIRECTORY} to ${CS_OUTPUT_DIRECTORY}")

	set(_log)
	foreach(shader IN LISTS _shaders)
		set(output_file ${shader}.spv)
		get_filename_component(filename ${shader} NAME)

		if(NOT EXISTS ${output_file} OR ${shader} IS_NEWER_THAN ${output_file})
			# msg(STATUS "compile_shaders: Compiling ${shader} to ${output_file}")
			execute_process(
				COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${shader} -o ${output_file}
				OUTPUT_VARIABLE _output
				RESULT_VARIABLE result
			)
			if(result)
				list(APPEND _log "compile_shaders: ❌ ${filename} -> ${filename}.spv (${result})")
				message(WARNING ${_output})
			else()
				list(APPEND _log "compile_shaders: ✅ ${filename} -> ${filename}.spv")
			endif()
		else()
			list(APPEND _log "compile_shaders: ♻️ ${filename} -> ${filename}.spv (up to date)")
		endif()
	endforeach()

	if (CS_SILENT)
		return()
	endif()

	foreach(line IN LISTS _log)
		message(STATUS ${line})
	endforeach()

endfunction()
