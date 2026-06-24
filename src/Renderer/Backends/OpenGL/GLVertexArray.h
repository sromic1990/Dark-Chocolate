// src/Renderer/Backends/OpenGL/GLVertexArray.h
#pragma once
#include "Renderer/Backends/OpenGL/GLBuffer.h"
#include "Renderer/Backends/OpenGL/Vertex.h"
#include <memory>
#include <vector>
#include <cstdint>


namespace DC 
{

	// GLVertexArray --- RAII wrapper for an OpenGL Vertex Array Object.
	//
	// A VAO records:
	//   - Which buffer is bound as GL_ARRAY_BUFFER per attribute.
	//   - The format of each attribute (count, type, stride, offset).
	//   - Which attributes are enabled.
	//

    // Binding the VAO at draw time restores all this state in one call.
    class GLVertexArray 
    {
    public:
        GLVertexArray();
        ~GLVertexArray();


        GLVertexArray(const GLVertexArray&) = delete;
        GLVertexArray& operator=(const GLVertexArray&) = delete;
        GLVertexArray(GLVertexArray&&) = delete;
        GLVertexArray& operator=(GLVertexArray&&) = delete;


        // Attach a vertex buffer and record its attribute layout into the VAO.
        // shared_ptr: VAO keeps the VBO alive. If the VBO is destroyed before
        // the VAO, the VAO's recorded buffer name becomes invalid.
        template<size_t N>
        void AddVertexBuffer(std::shared_ptr<GLBuffer> vbo,
            const std::array<Vertex::Attribute, N>& attrs);


        void AddVertexBuffer(std::shared_ptr<GLBuffer> vbo,
            const std::vector<Vertex::Attribute>& attrs);


        void Bind()   const;
        void Unbind() const;
        uint32_t GetID() const { return m_ID; }


    private:
        void ConfigureAttribute(const Vertex::Attribute& attr);


        uint32_t m_ID = 0;
        std::vector<std::shared_ptr<GLBuffer>> m_VBOs;
    };

    template<size_t N>
    void GLVertexArray::AddVertexBuffer(
        std::shared_ptr<GLBuffer> vbo,
        const std::array<Vertex::Attribute, N>& attrs)
    {
        std::vector<Vertex::Attribute> v(attrs.begin(), attrs.end());
        AddVertexBuffer(std::move(vbo), v);
    }

}