// src/Renderer/RenderCommand.h
#pragma once;
#include "Renderer/IRenderBackend.h"

namespace DC
{
	class RenderCommand
	{
	public:
		// Called once during Application Init. Raw non-owning pointer.
		// Application owns the backend via unique pointer.
		static void Init(IRenderBackend* backend) noexcept { s_Backend = backend; }

		// All calls are null-safe. Silently no-op when s_Backend is nullptr.
		// This supports headless testing and null-backend unit tests.
		static void BeginFrame(const ClearColor& clearColor = {})
		{
			if (s_Backend)
				s_Backend->BeginFrame(clearColor);
		}

		static void EndFrame()
		{
			if (s_Backend)
				s_Backend->EndFrame();
		}

		static void Present(bool vsync)
		{
			if (s_Backend)
				s_Backend->Present(vsync);
		}

		static void OnResize(uint32_t w, uint32_t h)
		{
			if (s_Backend)
				s_Backend->OnResize(w, h);
		}

		static void DrawTriangle()
		{
			if (s_Backend)
				s_Backend->DrawTriangle();
		}

		static std::string GetBackendName()
		{
			return s_Backend ? s_Backend->GetName() : "None";
		}
	
	private:
		static IRenderBackend* s_Backend;
	};
}
