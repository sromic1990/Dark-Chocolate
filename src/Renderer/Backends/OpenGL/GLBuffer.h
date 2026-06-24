// src/Renderer/Backends/OpenGL/GBuffer.h
#pragma once
#include <cstdint>
#include <cstddef>

namespace DC
{
	class GLBuffer
	{
	public:
		enum class Type 
		{
			Vertex,		// GL_ARRAY_BUFFER
			Index,		// GL_ELEMENT_ARRAY_BUFFER
		};

		explicit GLBuffer(Type type);
		~GLBuffer();

		GLBuffer(const GLBuffer&)				= delete;
		GLBuffer& operator=(const GLBuffer&)	= delete;
		GLBuffer(GLBuffer&& other)				noexcept;
		GLBuffer& operator=(GLBuffer&& other)	noexcept;

		// Upload data to buffer
		// sizeBytes: total byte count of the data block.
		// dynamic:		true = GL_DYNAMIC_DRAW (updated frequenctly).
		//				false = GL_STATIC_DRAW (updated once, drawn many times).
		void Upload(const void* data, size_t sizeBytes, bool dynamic = false);

		void Bind()				const;
		void Unbind()			const;
		uint32_t GetID()		const { return m_ID; }
		uint32_t GetTarget()	const { return m_Target; }

	private:
		uint32_t m_ID		= 0; // GL buffer name (0 = invalid)
		uint32_t m_Target	= 0; // GLenum: GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER
	};
}