// src/Renderer/Backends/OpenGL/OpenGLBackend.h
// MUST NOT include glad/glad.h.
// Including glad in a header forces all translation units that include
// this header to see GL function pointer declarations, which breaks
// headless unit tests. GL includes stay in the .cpp file only.
#pragma once
#include "Renderer/IRenderBackend.h"
#include <cstdint>
#include <memory>


namespace DC 
{
    class GLVertexArray;
    class GLShader;


    class OpenGLBackend final : public IRenderBackend 
	{
    public:
        OpenGLBackend() = default;
        ~OpenGLBackend() override;


        OpenGLBackend(const OpenGLBackend&) = delete;
        OpenGLBackend& operator=(const OpenGLBackend&) = delete;
        OpenGLBackend(OpenGLBackend&&) = delete;
        OpenGLBackend& operator=(OpenGLBackend&&) = delete;


        bool Init(void* windowHandle, uint32_t w, uint32_t h) override;
        void Shutdown()                                        override;
        void BeginFrame(const ClearColor& cc)                  override;

        void EndFrame()                                        override;
        void Present(bool vsync)                               override;
        void OnResize(uint32_t w, uint32_t h)                  override;
        std::string GetName() const                            override;
        void DrawTriangle()                                    override;


        // Receives triangle geometry from TriangleDemoLayer::OnAttach().
        // Called once after Init(); safe to call again to replace geometry.
        // This is the acknowledged temporary scaffolding seam (see Part 2b, Ch 9).
        void SetTriangleGeometry(std::shared_ptr<GLVertexArray> vao,
            std::shared_ptr<GLShader> shader);


    private:
        void* m_WindowHandle = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool     m_Initialised = false;
        bool m_Vsync = false;


        std::shared_ptr<GLVertexArray> m_TriangleVAO;
        std::shared_ptr<GLShader>      m_TriangleShader;
    };

}