cmake_minimum_required(VERSION 3.26)

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
file(GLOB_RECURSE SUBDIRECTORIES 
    "${CMAKE_SOURCE_DIR}/src/*.h" 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
source_group("src" "${CMAKE_SOURCE_DIR}/src" ${SUBDIRECTORIES})

add_executable(Vulkan-Basic 
    ${SUBDIRECTORIES}
)

# INCLUDE DIRECTORIES AND LINK THE LIBRARIES
target_include_directories(Vulkan-Basic PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(Vulkan-Basic PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(Vulkan-Basic PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(Vulkan-Basic PRIVATE "${CMAKE_SOURCE_DIR}/src/shaders")
target_link_libraries(Vulkan-Basic PRIVATE glfw)

# DEBUG AND RELEASE FLAGS
if(CMAKE_BUILD_TYPE STREQUAL "debug")
	target_compile_definitions(Vulkan-Basic PUBLIC DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
	target_compile_definitions(Vulkan-Basic PUBLIC NDEBUG)
endif()

# LOCATE GLSLC SHADER COMPILER FOR CROSS-PLATFORM USAGE
find_program(GLSLC_EXECUTABLE NAMES glslc
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/Bin32"        
)
if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "Could not find glslc. Make sure it's in your PATH or VULKAN_SDK is set correctly.")
else()
    message(STATUS "Using glslc at: ${GLSLC_EXECUTABLE}")
endif()

# COMPILE SHADERS FROM .vert/.frag TO .spv
file(GLOB SHADER_SOURCES
    "${CMAKE_SOURCE_DIR}/src/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/src/shaders/*.frag"
)

set(SPIRV_OUTPUTS "")

foreach(SHADER_FILE ${SHADER_SOURCES})
    # EXTRACT THE BASE FILENAME (e.g. "shader") AND EXTENSION (".vert" or ".frag")
    get_filename_component(FILE_NAME_WE ${SHADER_FILE} NAME_WE) # e.g. "shader"
    get_filename_component(FILE_EXT ${SHADER_FILE} EXT) # e.g. ".vert"

    # PUTTING THE COMPILED .spv FILES IN THE BUILD DIRECTORY UNDER /shaders/
    set(SPIRV_FILE
        "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME_WE}${FILE_EXT}.spv"
    )

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${SPIRV_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling ${SHADER_FILE} -> ${SPIRV_FILE}"
        VERBATIM
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_FILE})
endforeach()

# CREATE A CUSTOM TARGET THAT DEPENDS ON ALL SPIR-V SPIRV_OUTPUTS
add_custom_target(CompileShaders ALL
    DEPENDS ${SPIRV_OUTPUTS}
)

# MAKE THE MAIN EXECUTIBLE DEPEND ON THESE COMPILED SHADERS
add_dependencies(Vulkan-Basic CompileShaders)