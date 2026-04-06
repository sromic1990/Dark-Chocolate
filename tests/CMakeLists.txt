# tests/CMakeLists.txt


set(DC_TEST_SOURCES
    test_listener.cpp     # Catch2 session listener: initialises Log
    test_log.cpp
    test_window.cpp
    test_layerstack.cpp
    test_rendercommand.cpp
    test_application.cpp
    test_entity.cpp
    test_scene.cpp
    test_transform.cpp
    test_editormode.cpp
    test_opengl_unit.cpp
)


add_executable(DarkChocolateTests ${DC_TEST_SOURCES})


target_link_libraries(DarkChocolateTests PRIVATE
    DarkChocolateCore
    Catch2::Catch2WithMain)   # Provides main(). No test_main.cpp needed.


# Register with CTest. catch_discover_tests finds all TEST_CASEs
# in the binary and registers them individually with CTest.
# "cmake --build . --target test" runs all of them.
catch_discover_tests(DarkChocolateTests PROPERTIES TIMEOUT 30)   # 30-second per-test time limit

