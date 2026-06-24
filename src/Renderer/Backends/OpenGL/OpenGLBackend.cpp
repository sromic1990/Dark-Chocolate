// src/Renderer/Backends/OpenGL/OpenGLBackend.cpp
#include <glad/glad.h>  // MUST come before glfw3.h
#include <GLFW/glfw3.h>
#include "Renderer/Backends/OpenGL/OpenGLBackend.h"
#include "Renderer/Backends/OpenGL/GLVertexArray.h"
#include "Renderer/Backends/OpenGL/GLShader.h"
#include "Core/Log.h"
#include <stdexcept>


namespace DC 
{

    OpenGLBackend::~OpenGLBackend() 
	{
        if (m_Initialised) Shutdown();
    }


    bool OpenGLBackend::Init(void* windowHandle, uint32_t w, uint32_t h) 
	{
        if (m_Initialised) 
        {
            DC_CORE_WARN("OpenGLBackend::Init called twice --- ignoring.");
            return true;
        }
        m_WindowHandle = windowHandle;
        m_Width = w; m_Height = h;


        // Load all GL function pointers via GLAD.
        // glfwGetProcAddress is GLFW's platform function resolver.
        // MUST be called after glfwMakeContextCurrent() --- Window::Init()
        // called that before Application called our Init(), so ordering is
        // guaranteed.
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
        {
            DC_CORE_ERROR("GLAD failed to load OpenGL function pointers.");
            return false;
        }


        DC_CORE_INFO("OpenGL {} loaded on {} / {}",
            reinterpret_cast<const char*>(glGetString(GL_VERSION)),
            reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
            reinterpret_cast<const char*>(glGetString(GL_RENDERER)));


        // Set the initial viewport to the full window size.
        // The viewport maps NDC coordinates to pixel coordinates.
        // Without this call the default is correct (from window creation),
        // but being explicit documents intent.
        glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

        m_Initialised = true;
        return true;
    }


    void OpenGLBackend::Shutdown() 
	{
        // OpenGL context cleanup is handled by GLFW (glfwDestroyWindow).
        // GPU objects owned by RAII wrappers are destroyed before Shutdown()

		// is called --- the layer stack is cleared before backend shutdown
		// in Application.cpp.
        DC_CORE_INFO("OpenGLBackend shutdown.");
        m_Initialised = false;
    }


    void OpenGLBackend::BeginFrame(const ClearColor& cc) 
	{
        // glClearColor sets the colour used by glClear.
        // Must be called before glClear, not once at startup.
        glClearColor(cc.r, cc.g, cc.b, cc.a);
        glClear(GL_COLOR_BUFFER_BIT);
        // GL_DEPTH_BUFFER_BIT added in Part 3 when depth testing is enabled.
    }


    void OpenGLBackend::EndFrame() 
	{
        // OpenGL has no explicit EndFrame concept.
        // DX12/Vulkan close and submit command lists here.
        // OpenGL submits commands implicitly as they are issued.
        // This method exists purely to satisfy the IRenderBackend contract.
    }


    void OpenGLBackend::Present(bool vsync) 
	{
        auto* win = static_cast<GLFWwindow*>(m_WindowHandle);
        // Set vsync state before swap. 0 = unlimited, 1 = sync to vertical blank.
        // Set here rather than in Init so the user can toggle vsync at runtime.
        if (vsync != m_Vsync)          // only call when it actually changes
        {
            glfwSwapInterval(vsync ? 1 : 0);
            m_Vsync = vsync;
        }
        glfwSwapBuffers(win);
    }


    void OpenGLBackend::OnResize(uint32_t w, uint32_t h) 
	{
        if (w == 0 || h == 0) return;  // minimised, skip
        m_Width = w; m_Height = h;
        // Update the viewport to match the new window size.
        // Without this, rendered content squishes or letterboxes after resize.
        glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    }


    std::string OpenGLBackend::GetName() const { return "OpenGL 4.1"; }



    void OpenGLBackend::DrawTriangle() 
	{
        if (!m_TriangleVAO || !m_TriangleShader)
        {
            DC_CORE_ERROR("DrawTriangle: VAO or shader is null, skipping.");
            return;
        }

        m_TriangleShader->Bind();
        m_TriangleVAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        m_TriangleVAO->Unbind();
        m_TriangleShader->Unbind();
    }


    void OpenGLBackend::SetTriangleGeometry(
        std::shared_ptr<GLVertexArray> vao,
        std::shared_ptr<GLShader> shader)
    {
        m_TriangleVAO = std::move(vao);
        m_TriangleShader = std::move(shader);
    }


}