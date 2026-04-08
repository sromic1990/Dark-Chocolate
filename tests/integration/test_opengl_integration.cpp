// tests/integration/test_opengl_integration.cpp
#include <catch2/catch_test_macros.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer/Backends/OpenGL/OpenGLBackend.h"
#include "Renderer/Backends/OpenGL/GLShader.h"
#include "Renderer/Backends/OpenGL/GLVertexArray.h"
#include "Renderer/Backends/OpenGL/GLBuffer.h"
#include "Renderer/Backends/OpenGL/Vertex.h"
#include "Core/Log.h"
#include <array>

// GLFW in initialized once per test run, not per test.
// Per-test GLFW init/terminate is expensive and crashes on macOS
// (Cocoa event loop must not be re-initialized repeatedly).
// Catc2 REQUIRE_NOTHROW wraps GLFW_VISIBILE=FALSE window creation, so window creation failures produce readable errors.

static GLFWwindow* s_TestWindow = nullptr;
static const int WIDTH = 64, HEIGHT = 64;

// Catch2 session listener: init GLFW once, create one shared hidden window.
#include <catch2/catch_session.hpp>
struct GLContextListener : Catch::EventListenerBase
{
	using EventListenerBase::EventListenerBase;
	void testRunStarting(Catch::TestRunInfo const&) override
	{
		DC::Log::Init();
		DC::Log::GetCoreLogger()->set_level(spdlog::level::off);
		REQUIRE(glfwInit());
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		s_TestWindow = glfwCreateWindow(WIDTH, HEIGHT, "DC Test", nullptr, nullptr);

		REQUIRE(s_TestWindow != nullptr);
		glfwMakeContextCurrent(s_TestWindow);
		REQUIRE(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
	}

	void testRunEnded(Catch::TestRunStats const&) override
	{
		if (s_TestWindow)
		{
			glfwDestroyWindow(s_TestWindow);
			s_TestWindow = nullptr;
		}
		glfwTerminate();
	}
};
CATCH_REGISTER_LISTENER(GLContextListener);

// --Backend lifecycle--


TEST_CASE("OpenGLBackend Init returns true with valid window", "[integration]") 
{
    DC::OpenGLBackend backend;
    REQUIRE(backend.Init(static_cast<void*>(s_TestWindow), WIDTH, HEIGHT));
    backend.Shutdown();
}


TEST_CASE("OpenGLBackend Init loads GL version string", "[integration]") 
{
    DC::OpenGLBackend backend;
    backend.Init(static_cast<void*>(s_TestWindow), WIDTH, HEIGHT);
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    REQUIRE(version != nullptr);
    REQUIRE(std::string(version).find("4.") != std::string::npos);
    backend.Shutdown();
}


TEST_CASE("BeginFrame and EndFrame generate no GL errors", "[integration]") 
{
    DC::OpenGLBackend backend;
    backend.Init(static_cast<void*>(s_TestWindow), WIDTH, HEIGHT);
    DC::ClearColor cc{ 0.1f, 0.1f, 0.15f, 1.0f };
    backend.BeginFrame(cc);
    backend.EndFrame();
    REQUIRE(glGetError() == static_cast<GLenum>(GL_NO_ERROR));
    backend.Shutdown();
}


TEST_CASE("OnResize is clean", "[integration]") 
{
    DC::OpenGLBackend backend;
    backend.Init(static_cast<void*>(s_TestWindow), WIDTH, HEIGHT);
    backend.OnResize(128, 128);
    REQUIRE(glGetError() == static_cast<GLenum>(GL_NO_ERROR));
    backend.Shutdown();
}


// --Shader compilation--


TEST_CASE("Triangle shader compiles and links successfully", "[integration]") 
{
    REQUIRE_NOTHROW({
        DC::GLShader shader(DC::k_TriangleVertexShader, DC::k_TriangleFragmentShader);
        REQUIRE(shader.GetID() != 0u);
        });
}


TEST_CASE("Invalid shader source throws runtime_error", "[integration]") 
{
    REQUIRE_THROWS_AS(
        DC::GLShader("NOT VALID GLSL", "ALSO NOT VALID"),
        std::runtime_error);
}


// --Buffer and VAO--


TEST_CASE("GLBuffer gets a valid non-zero GL name", "[integration]") 
{
    DC::GLBuffer vbo(DC::GLBuffer::Type::Vertex);
    REQUIRE(vbo.GetID() != 0u);
}


TEST_CASE("Vertex buffer upload generates no GL errors", "[integration]") 
{
    DC::GLBuffer vbo(DC::GLBuffer::Type::Vertex);
    vbo.Upload(DC::k_TriangleVertices.data(),
        DC::k_TriangleVertices.size() * sizeof(DC::Vertex));
    REQUIRE(glGetError() == static_cast<GLenum>(GL_NO_ERROR));
}


TEST_CASE("GLVertexArray gets a valid non-zero GL name", "[integration]") 
{
    DC::GLVertexArray vao;
    REQUIRE(vao.GetID() != 0u);
}


// --THE PIXEL READBACK TEST--
// Did the triangle actually appear?
// We clear to a known dark colour, draw the triangle, sample the centre.


TEST_CASE("Triangle draws non-background pixel at framebuffer centre", "[integration]") 
{
    DC::GLShader shader(DC::k_TriangleVertexShader, DC::k_TriangleFragmentShader);
    auto vbo = std::make_shared<DC::GLBuffer>(DC::GLBuffer::Type::Vertex);
    vbo->Upload(DC::k_TriangleVertices.data(),
        DC::k_TriangleVertices.size() * sizeof(DC::Vertex));
    DC::GLVertexArray vao;
    vao.AddVertexBuffer(vbo, DC::Vertex::GetAttributes());


    // Clear to known dark colour: R=25, G=25, B=38 (≈ 0.1, 0.1, 0.15 * 255)
    const float CR = 0.1f, CG = 0.1f, CB = 0.15f;
    glClearColor(CR, CG, CB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    shader.Bind(); vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vao.Unbind();


    // glFinish: ensure draw commands complete before readback.
    glFinish();


    // OpenGL pixel origin is bottom-left.
    // Centre of 64x64 framebuffer = (32, 32).
    std::array<uint8_t, 4> pixel{};
    glReadPixels(WIDTH / 2, HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());
    REQUIRE(glGetError() == static_cast<GLenum>(GL_NO_ERROR));


    // Triangle centre ≈ barycentric average of red/green/blue = grey.
    // Each channel should differ from the dark clear colour by > 50/255.
    const uint8_t clearR = static_cast<uint8_t>(CR * 255.0f);
    const uint8_t clearG = static_cast<uint8_t>(CG * 255.0f);
    const uint8_t clearB = static_cast<uint8_t>(CB * 255.0f);


    bool pixelDiffers =
        std::abs(int(pixel[0]) - int(clearR)) > 50 ||
        std::abs(int(pixel[1]) - int(clearG)) > 50 ||
        std::abs(int(pixel[2]) - int(clearB)) > 50;


    INFO("Centre pixel RGBA: " << int(pixel[0]) << "," << int(pixel[1])
        << "," << int(pixel[2]) << "," << int(pixel[3]));
    REQUIRE(pixelDiffers);
}

