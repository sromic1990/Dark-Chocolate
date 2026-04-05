# src/CMakeLists.txt


add_library(DarkChocolateCore STATIC
    core/Application.cpp
    core/Window.cpp
    core/LayerStack.cpp
    core/Log.cpp
    renderer/IRenderBackend.cpp
    renderer/RenderCommand.cpp
    scene/Scene.cpp
    editor/EditorLayer.cpp)


# PUBLIC: visible to DarkChocolateCore AND to any target linking it.
target_include_directories(DarkChocolateCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(DarkChocolateCore PUBLIC glfw glm::glm spdlog::spdlog imgui)


# Platform compile definitions.
if(WIN32)
    target_compile_definitions(DarkChocolateCore PUBLIC DC_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(DarkChocolateCore PUBLIC DC_PLATFORM_MACOS)
elseif(UNIX)
    target_compile_definitions(DarkChocolateCore PUBLIC DC_PLATFORM_LINUX)
endif()

target_compile_definitions(DarkChocolateCore PUBLIC GLM_ENABLE_EXPERIMENTAL)

add_executable(DarkChocolate core/EntryPoint.cpp editor/EditorApp.cpp)
target_link_libraries(DarkChocolate PRIVATE DarkChocolateCore)


# macOS framework links (Cocoa/IOKit/CoreVideo for GLFW).
if(APPLE)
    target_link_libraries(DarkChocolate PRIVATE
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

