// src/Renderer/Backends/OpenGL/Vertex.h
// Intentionally self-contained, depends only on GLM and stdlib.
// No OpenGL headers. No engine headers.
// WHY? Vertex.h is included by unit tests that run without a GL context.
// Including glad.h here would drag unsolved GL function pointers into every file that includes Vertex.h.
// That will cause linker errors in tests.
#pragma once
#include <glm/glm.hpp>
#include <array>
#include <cstdint>
#include <cstddef>

namespace DC
{
	// Memory layout (24 bytes, no padding):
	// offset 0: Position.x (float, 4 bytes)
	// offset 4: Position.y (float, 4 bytes)
	// offset 8: Position.z (float, 4 bytes)
	// offset 12: Color.r (float, 4 bytes)
	// offset 16: Color.g (float, 4 bytes)
	// offset 20: Color.b (float, 4 bytes)
	//
	// GPU shader binds:
	//		layout(location = 0) in vec3 a_Positions; // offset 0, stride 24
	//		layout(location = 1) in vec3 a_Color; // offset 12, stride 24
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;

		// Vertex attribute descriptors for glVertexAttribPointer.
		// Keeps layout knowledge co-located with the struct.
		struct Attribute 
		{
			uint32_t  Index;       // layout(location = N)
			int32_t   Count;       // number of components (2, 3, or 4)
			uint32_t  GLType;      // GL_FLOAT = 0x1406 (stable ABI constant)
			bool      Normalised;  // normalise integer types?
			uint32_t  Stride;      // bytes between consecutive vertices
			uintptr_t Offset;      // byte offset of this attribute in the struct
		};


		static std::array<Attribute, 2> GetAttributes() 
		{
			return { {
					// Position: location 0, 3 floats, not normalised
					{ 0, 3, 0x1406 /*GL_FLOAT*/, false,
					  sizeof(Vertex), offsetof(Vertex, Position) },
					  // Color: location 1, 3 floats, not normalised
					  { 1, 3, 0x1406 /*GL_FLOAT*/, false,
						sizeof(Vertex), offsetof(Vertex, Color) },
				  } };
		}

	};

	// Compile-time layout verification.
	// If the compiler inserts padding between Position and Color,
	// this fires with a clear error rather than silent CPU corruption.
	static_assert(sizeof(Vertex) == 24, "Vertex has unexpected padding. GPU attribute offsets will be wrong.");

	// Triangle vertices in NDC. Top=red, bottom-right=green, bottom-left=blue.
	// constexpr: evaluated at compile time, stored in read-only data segment.
	// inline: permits multiple-translation-unit definition (required in headers).
	inline constexpr std::array<Vertex, 3> k_TriangleVertices = 
	{ {
		{{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	} };

	// Raw GLSL strings for the triangle shader pair.
	// TEMPORARY: these live here to keep all of it self-contained for now.
	// (hot-reload, SPIRV cross-compilation to HLSL/MSL).
	inline constexpr std::string_view k_TriangleVertexShader = 
		R"(
			#version 410 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Color;
			out vec3 v_Color;
			void main() 
			{
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

	inline constexpr std::string_view k_TriangleFragmentShader = 
		R"(
			#version 410 core
			in vec3 v_Color;
			out vec4 FragColor;
			void main() 
			{
				FragColor = vec4(v_Color, 1.0);
			}
		)";
}
