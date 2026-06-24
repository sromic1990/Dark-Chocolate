// src/Renderer/Backends/OpenGL/GLVertexArray.cpp
#include <glad/glad.h>
#include "Renderer/Backends/OpenGL/GLVertexArray.h"
#include "Core/Log.h"


namespace DC 
{
    GLVertexArray::GLVertexArray() 
	{
        glGenVertexArrays(1, &m_ID);
        DC_CORE_TRACE("GLVertexArray created (ID={}).", m_ID);
    }


    GLVertexArray::~GLVertexArray() 
	{
        if (m_ID != 0) glDeleteVertexArrays(1, &m_ID);
    }


    void GLVertexArray::AddVertexBuffer(
        std::shared_ptr<GLBuffer> vbo,
        const std::vector<Vertex::Attribute>& attrs)
    {
        // Everything between Bind() and Unbind() is recorded by the VAO.
        // The VAO stores references to buffer names, not the data itself.
        Bind();
        vbo->Bind();
        for (const auto& attr : attrs) ConfigureAttribute(attr);
        vbo->Unbind();
        Unbind();
        m_VBOs.push_back(std::move(vbo));
    }


    void GLVertexArray::ConfigureAttribute(const Vertex::Attribute& attr) 
	{
        // glEnableVertexAttribArray: activate the attribute slot.
        // Without this, the attribute exists in the shader but always

		// reads zero --- no error, silent wrong output.
        glEnableVertexAttribArray(attr.Index);


        // glVertexAttribPointer: describe one vertex attribute.
        // index:      which shader attribute (layout(location = N))
        // size:       number of components (2=vec2, 3=vec3, 4=vec4)
        // type:       component type (GL_FLOAT = 0x1406)
        // normalised: for integers: convert [0,255] -> [0.0, 1.0]?
        // stride:     byte distance between consecutive vertices
        // pointer:    byte offset of first component in the buffer
        glVertexAttribPointer(
            attr.Index,
            attr.Count,
            static_cast<GLenum>(attr.GLType),
            attr.Normalised ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(attr.Stride),
            reinterpret_cast<const void*>(attr.Offset));
    }


    void GLVertexArray::Bind()   const { glBindVertexArray(m_ID); }
    void GLVertexArray::Unbind() const { glBindVertexArray(0); }


}
