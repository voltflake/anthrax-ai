
# Function: aai_scan_demos
#
# This function scans a given directory for demo directories and returns a list of them.
# A demo directory is defined as a directory that matches the pattern "demo-.*" and contains a CMakeLists.txt file.
# If no directory is provided, the function will use the current source directory.
#
# Parameters:
#   RESULT - The variable in which to store the result. This will be a list of demo directories found.
#   DIRECTORY - The directory to scan for demo directories. If not provided, the current source directory is used.
#
# Usage:
#   aai_scan_demos(RESULT result_var DIRECTORY directory_to_scan)
#
# After calling this function, result_var will contain a list of demo directories found in directory_to_scan.
function(aai_scan_demos)
	include(CMakeParseArguments)

	cmake_parse_arguments(AAISD "" "RESULT;DIRECTORY" "" ${ARGN})
	if (NOT AAISD_DIRECTORY)
		set(AAISD_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	message(STATUS "Scanning for demos in ${AAISD_DIRECTORY}")
	file(GLOB directories RELATIVE ${AAISD_DIRECTORY} ${AAISD_DIRECTORY}/*)

	set(result)
	foreach(dir IN LISTS directories)
		if (NOT (${dir} MATCHES "demo-.*"))
			continue()
		endif()
		if (NOT EXISTS ${AAISD_DIRECTORY}/${dir}/CMakeLists.txt)
			continue()
		endif()

		list(APPEND result ${dir})
	endforeach()

	list(REMOVE_DUPLICATES result)
	list(LENGTH result count)
	message(STATUS "Found ${count} demos:")
	foreach(dir IN LISTS result)
		message(STATUS "> ${dir}")
	endforeach()

	set(${AAISD_RESULT} ${result} PARENT_SCOPE)
endfunction(aai_scan_demos)
