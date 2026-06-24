// src/Editor/TriangleDemoLayer.cpp
#include "Editor/TriangleDemoLayer.h"
#include "Renderer/Backends/OpenGL/Vertex.h"
#include "Renderer/Backends/OpenGL/OpenGLBackend.h"
#include "Renderer/RenderCommand.h"
#include "Core/Log.h"
#include <imgui.h>


namespace DC 
{

	TriangleDemoLayer::TriangleDemoLayer()
        : Layer("TriangleDemoLayer")
    {}


    void TriangleDemoLayer::OnAttach() 
	{
        DC_CORE_INFO("TriangleDemoLayer attached --- creating GPU objects.");


        // 1. Compile and link the shader.
        //    GLShader throws with the full GLSL log on error.
        m_Shader = std::make_unique<GLShader>(
            k_TriangleVertexShader, k_TriangleFragmentShader);


        // 2. Upload vertex data.
        //    72 bytes total (3 vertices * 24 bytes each).
        m_VertexBuffer = std::make_shared<GLBuffer>(GLBuffer::Type::Vertex);
        m_VertexBuffer->Upload(
            k_TriangleVertices.data(),
            k_TriangleVertices.size() * sizeof(Vertex));


        // 3. Configure the vertex array (records attribute layout into VAO).
        m_VertexArray = std::make_unique<GLVertexArray>();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer, Vertex::GetAttributes());


        DC_CORE_INFO("TriangleDemoLayer GPU objects ready.");


        // Hand geometry references to the backend so DrawTriangle() can use them.
        // Acknowledged temporary scaffolding --- replaced by mesh renderer in Part 7.
        // dynamic_cast: safe here because we are always on the OpenGL backend in Part 2c.
        // If the cast returns nullptr (different backend), DrawTriangle() no-ops.
        if (auto* gl = dynamic_cast<OpenGLBackend*>(RenderCommand::GetRawBackend())) 
        {
            DC_CORE_INFO("dynamic_cast succeeded, setting triangle geometry.");
            gl->SetTriangleGeometry(
                std::shared_ptr<GLVertexArray>(m_VertexArray.get(), [](auto*) {}),
                std::shared_ptr<GLShader>(m_Shader.get(), [](auto*) {}));
        }
        else
        {
            DC_CORE_ERROR("dynamic_cast FAILED, triangle geometry not set.");
        }
    }


    void TriangleDemoLayer::OnDetach() 
	{
        DC_CORE_INFO("TriangleDemoLayer detached --- releasing GPU objects.");
        // Explicit reset order for clarity.
        // Shader does not reference the VAO or buffer, so it can be released first.
        // The VAO holds a shared_ptr to the buffer, so buffer lives until VAO is reset.
        m_Shader.reset();
        m_VertexArray.reset();

        m_VertexBuffer.reset();
    }


    void TriangleDemoLayer::OnRender() 
	{
        // Routes through RenderCommand so swapping to DX11/DX12/Vulkan/Metal
        // requires zero changes in this file. That is the entire point.
        RenderCommand::DrawTriangle();
    }


    void TriangleDemoLayer::OnImGuiRender() 
	{
        // Stats overlay only. No GL state touched here.
        ImGui::Begin("Triangle Debug");
        ImGui::Text("Backend : %s", RenderCommand::GetBackendName().c_str());
        ImGui::Text("Vertices: 3 | Draw calls: 1 per frame");
        ImGui::End();
    }

}