cmake_minimum_required (VERSION 3.26)

# PROJECT NAME
project ("Vulkan-Basic")

# SETTING CMAKE OPTIONS
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)

# DETECT COMPILER AND VERSION
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.30")
		message(FATAL_ERROR "MSVC 19.30+ is required for Vulkan.")
	endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0")
		message(FATAL_ERROR "GCC 11.0+ is required for Vulkan.")
	endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13.0")
		message(FATAL_ERROR "Clang 13.0+ is required for Vulkan.")
	endif()
else()
	message(WARMING "Unknown compiler. Ensure it supports Vulkan.")
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")

# SET THE OUTPUT DIRECTORY
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# FETCH EXTERNAL LIBRARIES
include(libs/FetchLibraries.cmake)

# ADD THE SOURCE SUBDIRECTORY
add_subdirectory("src")
