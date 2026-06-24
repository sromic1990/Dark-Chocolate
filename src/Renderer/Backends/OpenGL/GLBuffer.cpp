// src/Renderer/Backends/OpenGL/GLBuffer.cpp

#include <glad/glad.h>
#include "Renderer/Backends/OpenGL/GLBuffer.h"
#include "Core/Log.h"
#include <utility>

namespace DC
{
	static uint32_t ToGLTarget(GLBuffer::Type type)
	{
		switch (type)
		{
			case GLBuffer::Type::Vertex: return GL_ARRAY_BUFFER;
			case GLBuffer::Type::Index: return GL_ELEMENT_ARRAY_BUFFER;
		}
		return GL_ARRAY_BUFFER;
	}

	GLBuffer::GLBuffer(Type type) : m_Target(ToGLTarget(type))
	{
		// glGenBuffers reserves a name. No GPU memory is allocated yet.
		// Memory is allocated by glBufferData in Upload()
		glGenBuffers(1, &m_ID);
		DC_CORE_TRACE("GLBuffer created (ID = {}).", m_ID);
	}

	GLBuffer::~GLBuffer()
	{
		// glDeleteBuffers with ID = 0 is a documented no-op.
		// Safe after a move (m_ID is set to 0 in move operations).
		if (m_ID != 0) { glDeleteBuffers(1, &m_ID); }
	}

	// Move constructor: transfer ownership using std::exchange.
	// std::exchange(other.m_ID, 0) returns the old value AND sets other.m_ID to 0.
	// When "other" is later destroyed, it calls glDeleteBuffers(1, &0) --- a no-op.
	// Without this, "other"'s destructor would delete our buffer.
	GLBuffer::GLBuffer(GLBuffer&& other) noexcept
		: m_ID(std::exchange(other.m_ID, 0))
		, m_Target(other.m_Target)
	{}

	GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept 
	{
		if (this != &other) 
		{
			if (m_ID != 0) glDeleteBuffers(1, &m_ID);
			m_ID = std::exchange(other.m_ID, 0);
			m_Target = other.m_Target;
		}
		return *this;
	}

	void GLBuffer::Bind()   const { glBindBuffer(m_Target, m_ID); }
	void GLBuffer::Unbind() const { glBindBuffer(m_Target, 0); }

	void GLBuffer::Upload(const void* data, size_t sizeBytes, bool dynamic) 
	{
		Bind();
		// glBufferData allocates GPU memory and uploads the data.
		// GL_STATIC_DRAW: hint that data is uploaded once, drawn many times.
		// GL_DYNAMIC_DRAW: hint that data is updated frequently.
		// The driver uses these hints for memory placement (VRAM vs shared).
		glBufferData(m_Target, static_cast<GLsizeiptr>(sizeBytes),
			data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		Unbind();
	}

}