// tests/test_opengl_unit.cpp
#include <catch2/catch_test_macros.hpp>
#include<glm/glm.hpp>
#include <cstddef>
#include "Renderer/Backends/OpenGL/Vertex.h"
#include "Renderer/Backends/OpenGL/OpenGLBackend.h"
#include "Editor/TriangleDemoLayer.h"

// --Vertex struct layout--
// The GPU driver uses byte offsets to find attribute data
// If sizeof or offsetof are wrong, the vertex shader reads garbage.
// These tests catch layout errors before any GPU is involved.

// Vertex is two vec3s back-to-back, first position then color.
// sizeof(glm::vec3) == 12 bytes (3 floats * 4 bytes each)
// sizeof(Vertex) must exactly be 24 bytes with no padding.
TEST_CASE("Vertex struct size is exactly 24 bytes", "[vertex]")
{
	REQUIRE(sizeof(DC::Vertex)== 24u);
}

TEST_CASE("Vertex Position is at byte offset 0", "[vertex]")
{
	REQUIRE(offsetof(DC::Vertex, Position) == 0u);
}

TEST_CASE("Vertex Color is at byte offset 12", "[vertex]")
{
	REQUIRE(offsetof(DC::Vertex, Color) == 12u);
}

TEST_CASE("Triangle vertices are in NDC", "[vertex]")
{
	for (const auto& v : DC::k_TriangleVertices)
	{
		REQUIRE(v.Postion.x >= -1.0f);
		REQUIRE(v.Postion.x <= 1.0f);
		REQUIRE(v.Postion.y >= -1.0f);
		REQUIRE(v.Postion.y <= 1.0f);
	}
}

TEST_CASE("Triangle has exactly 3 vertices", "[vertex]")
{
	REQUIRE(DC::k_TriangleVertices.size() == 3u);
}

TEST_CASE("Vertex colors are in normalized range [0, 1]", "[vertex]")
{
	// Outside [0, 1]: behaviour depends on framebuffer format (clamp or HDR).
	for (const auto& v : DC::k_TriangleVertices)
	{
		REQUIRE(v.Color.r >= 0.0f);
		REQUIRE(v.Color.r <= 1.0f);
		REQUIRE(v.Color.g >= 0.0f);
		REQUIRE(v.Color.g <= 1.0f);
		REQUIRE(v.Color.b >= 0.0f);
		REQUIRE(v.Color.b <= 1.0f);
	}
}

// --Shader source strings--
// Cannot compile without a GL context, but can verify non-empty, and contain required keywords.
// An empty source silently breaks glCompileShader.

TEST_CASE("Vertex shader source is non-empty", "[shader]")
{
	REQUIRE_FALSE(DC::k_TriangleVertexShader.empty());
}

TEST_CASE("Fragment shader source is non-empty", "[shader]")
{
	REQUIRE_FALSE(DC::k_TriangleFragmentShader.empty());
}

TEST_CASE("Vertex shader contains gl_Position", "[shader]")
{
	REQUIRE(DC::k_TriangleVertexShader.find("gl_Position") != std::string_view::npos);
}

TEST_CASE("Fragment shader contains FragColor output", "[shader]")
{
	REQUIRE(DC::k_TriangleFragmentShader.find("FragColor") != std::string_view::npos);
}

TEST_CASE("Vertex shader declares attribute at location 0", "[shader]")
{
	REQUIRE(DC::k_TriangleVertexShader.find("location = 0") != std::string_view::npos);
}

TEST_CASE("Vertex shader declares attribute at location 1", "[shader]")
{
	REQUIRE(DC::k_TriangleVertexShader.find("location = 1") != std::string_view::npos);
}

// --Backend construction (no GL context needed)--
TEST_CASE("OpenGLBackend can be constructed without a GL context", "[opengl]")
{
	// Construction must not call any GL function.
	// GL function pointers are null before gladLoadGLLoader.
	// If the constructor calls a GL function, it crashes immediately.
	REQUIRE_NOTHROW([] {DC::OpenGLBackend backend; }());
}

TEST_CASE("OpenGLBackend GetName returns OpenGL 4.1", "[opengl]")
{
	DC::OpenGLBackend backend;
	REQUIRE(backend.GetName() == "OpenGL 4.1");
}

// --TriangleDemoLayer construction (no GL context needed)--
TEST_CASE("TriangleDemoLayer can be constructed without GL context", "[demo]")
{
	// GL objects are created in OnAttach, not the constructor.
	REQUIRE_NOTHROW([] {DC::TriangleDemoLayer layer; }());
}

TEST_CASE("TriangleDemoLayer GetName is correct", "[demo]")
{
	DC::TriangleDemoLayer layer;
	REQUIRE(layer.GetName() == "TriangleDemoLayer");
}