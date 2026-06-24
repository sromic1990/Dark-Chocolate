// src/Editor/TriangleDemoLayer.h
#pragma once
#include "Core/Layer.h"
#include "Renderer/Backends/OpenGL/GLShader.h"
#include "Renderer/Backends/OpenGL/GLVertexArray.h"
#include "Renderer/Backends/OpenGL/GLBuffer.h"
#include <memory>


namespace DC 
{
	// TriangleDemoLayer: draws a coloured triangle every frame.
	// Owns: GLShader, GLVertexArray, GLBuffer.
	//
	// WHY unique_ptr, not value members?
	// GL objects cannot be constructed without an active GL context.
	// The context exists after OpenGLBackend::Init(), which runs before
	// OnAttach(). Constructing GL objects in the Layer constructor would
    // crash because the GL function pointers are not yet loaded.
	// unique_ptr, initialised to nullptr, populated in OnAttach = correct.
    class TriangleDemoLayer : public Layer 
	{
    public:
        TriangleDemoLayer();
        ~TriangleDemoLayer() override = default;


        void OnAttach()      override;
        void OnDetach()      override;
        void OnRender()      override;  // draw call lives here (not OnImGuiRender)
        void OnImGuiRender() override;  // stats overlay only


    private:
        std::unique_ptr<GLShader>      m_Shader;
        std::unique_ptr<GLVertexArray> m_VertexArray;
        std::shared_ptr<GLBuffer>      m_VertexBuffer;
    };

}