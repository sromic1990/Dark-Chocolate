// src/Renderer/Backends/OpenGL/GLShader.h
#pragma once
#include <string>
#include <string_view>
#include <cstdint>


namespace DC 
{
    // GLShader: RAII wrapper for an OpenGL shader program.
    // Compiles vertex + fragment shaders, links into a program.
    // Throws std::runtime_error (with full GLSL error log) on any failure.
    class GLShader 
    {
    public:
        // WHY string_view? Accepts std::string, const char*, string_view,
        // and constexpr string_view without copying.
        GLShader(std::string_view vertSrc, std::string_view fragSrc);
        ~GLShader();


        GLShader(const GLShader&) = delete;
        GLShader& operator=(const GLShader&) = delete;
        GLShader(GLShader&&) = delete;
        GLShader& operator=(GLShader&&) = delete;


        void Bind()   const;
        void Unbind() const;
        uint32_t GetID() const { return m_ID; }


    private:
        static uint32_t CompileStage(uint32_t stageType, std::string_view src);
        uint32_t m_ID = 0;
    };
}