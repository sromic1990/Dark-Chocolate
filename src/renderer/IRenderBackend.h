// src/Renderer/IRenderBackend.h
#pragma once
#include <cstdint>
#include <string>
#include <memory>

namespace DC
{
	enum class BackendType
	{
		None = 0,
		DX11 = 1,
		DX12 = 2,
		Vulkan = 3,
		Metal = 4,
		OpenGL = 5,
	};


	struct ClearColor
	{
		float r = 0.1f;
		float g = 0.1f;
		float b = 0.1f;
		float a = 1.0f;
	};

	class IRenderBackend
	{
	public:
		virtual ~IRenderBackend()										= default;
		virtual bool Init(void* windowHandle, uint32_t w, uint32_t h)	= 0;
		virtual void Shutdown()											= 0;
		virtual void BeginFrame(const ClearColor& clearColor)			= 0;
		virtual void EndFrame()											= 0;
		virtual void Present(bool vSync)								= 0;
		virtual void OnResize(uint32_t w, uint32_t h)					= 0;
		virtual std::string GetName() const								= 0;
		virtual void DrawTriangle()										= 0;

		static std::unique_ptr<IRenderBackend> Create(BackendType type);
	};
}