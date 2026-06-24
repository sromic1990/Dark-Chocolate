// src/Renderer/Backends/OpenGL/GLShader.cpp
#include <glad/glad.h>
#include "Renderer/Backends/OpenGL/GLShader.h"
#include "Core/Log.h"
#include <stdexcept>
#include <vector>


namespace DC 
{

    uint32_t GLShader::CompileStage(uint32_t stageType, std::string_view src) 
	{
        if (src.empty())
            throw std::runtime_error("GLShader: empty shader source.");

        uint32_t id = glCreateShader(static_cast<GLenum>(stageType));

        // glShaderSource accepts C-string. string_view is not null-terminated.
        // We pass the length explicitly --- no null terminator required.
        const char* srcPtr = src.data();
        const GLint srcLen = static_cast<GLint>(src.size());
        glShaderSource(id, 1, &srcPtr, &srcLen);
        glCompileShader(id);


        GLint success = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) 
        {
            // Query log length first, allocate exactly, then fetch.
            GLint logLen = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
            std::vector<char> log(static_cast<size_t>(logLen));
            glGetShaderInfoLog(id, logLen, nullptr, log.data());
            glDeleteShader(id);  // no resource leak before throw
            const char* stageName = (stageType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
            throw std::runtime_error(std::string("GLShader ") + stageName
                + " compile error:\n" + log.data());
        }
        return id;
    }


    GLShader::GLShader(std::string_view vertSrc, std::string_view fragSrc) 
	{
        uint32_t vert = CompileStage(GL_VERTEX_SHADER, vertSrc);
        uint32_t frag = CompileStage(GL_FRAGMENT_SHADER, fragSrc);


        m_ID = glCreateProgram();
        glAttachShader(m_ID, vert);
        glAttachShader(m_ID, frag);
        glLinkProgram(m_ID);


        GLint linked = GL_FALSE;
        glGetProgramiv(m_ID, GL_LINK_STATUS, &linked);

        if (!linked) 
        {
            GLint logLen = 0;
            glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &logLen);
            std::vector<char> log(static_cast<size_t>(logLen));
            glGetProgramInfoLog(m_ID, logLen, nullptr, log.data());
            glDeleteShader(vert); glDeleteShader(frag); glDeleteProgram(m_ID); m_ID = 0;
            throw std::runtime_error(std::string("GLShader link error:\n") + log.data());
        }


        // Shader stages can be detached and deleted after linking.
        // The program retains the compiled executable internally.
        glDetachShader(m_ID, vert); glDetachShader(m_ID, frag);
        glDeleteShader(vert);       glDeleteShader(frag);

        DC_CORE_INFO("GLShader compiled and linked (ID={}).", m_ID);
    }

    GLShader::~GLShader() 
	{
        if (m_ID != 0) glDeleteProgram(m_ID);
    }

    void GLShader::Bind()   const { glUseProgram(m_ID); }
    void GLShader::Unbind() const { glUseProgram(0); }

}