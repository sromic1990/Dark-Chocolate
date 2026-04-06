# tests/Integration/CMakeLists.txt

add_executable(DarkChocolateIntegrationTests
    test_opengl_integration.cpp)

target_link_libraries(DarkChocolateIntegrationTests PRIVATE
    DarkChocolateCore
    Catch2::Catch2WithMain
    glad
    glfw
    OpenGL::GL)

# 60-second timeout: context can be slow under Mesa
catc_discover_tests(DarkChocolateIntegrationTests PROPERTIES TIMEOUT 60)