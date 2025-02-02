cmake_minimum_required (VERSION 3.26)

# POPULATE 3RD PARTY LIBRARIES
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)

# ENSURE VULKAN IS INSTALLED
if(APPLE)
	set(VULKAN_SDK $ENV{VULKAN_SDK})
    set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")
    set(Vulkan_LIB_DIR "${VULKAN_SDK}/Lib")
    set(Vulkan_LIBRARY "${Vulkan_LIB_DIR}/libMoltenVK.dylib")
    include_directories(${Vulkan_INCLUDE_DIR})
    link_directories(${Vulkan_LIB_DIR})
else()
    # Manually specify Vulkan paths
    set(VULKAN_SDK "C:/VulkanSDK/1.4.304.0")  
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK}/Lib/vulkan-1.lib")

    # Add Vulkan to your include directories
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${VULKAN_SDK}/Lib)

    # Debug output
    message(STATUS "Manually set Vulkan SDK")
    message(STATUS "Vulkan Include Dirs: ${Vulkan_INCLUDE_DIRS}")
    message(STATUS "Vulkan Libraries: ${Vulkan_LIBRARIES}")
endif()

# CREATE EXECUTABLE
file(GLOB_RECURSE SUBDIRECTORIES "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.cpp")
source_group("src" "${CMAKE_SOURCE_DIR}/src" ${SUBDIRECTORIES})
add_executable(Vulkan-Basic main.cpp)
target_include_directories(Vulkan-Basic PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Vulkan-Basic PRIVATE ${Vulkan_LIBRARIES})

# INCLUDE DIRECTORIES AND LINK LIBRARIES
target_include_directories(Vulkan-Basic PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(Vulkan-Basic PRIVATE glfw)

# DEBUG AND RELEASE FLAGS
if(CMAKE_BUILD_TYPE STREQUAL "debug")
	target_compile_definitions(Vulkan-Basic PUBLIC DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
	target_compile_definitions(Vulkan-Basic PUBLIC NDEBUG)
endif()