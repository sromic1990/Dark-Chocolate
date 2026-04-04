// src/Renderer/IRenderBackend.cpp
#include "Renderer/IRenderBackend.h"
#include "Core/Log.h"

// Uncomment as each backend is implemented
// #include "Renderer/Backends/OpenGL/OpenGLBackend.h"
// #ifdef DC_PLATFORM_WINDOWS
// #include "Renderer/Backends/DX11/DX11Backend.h"
// #include "Renderer/Backends/DX12/DX12Backend.h"
// #endif
// #include "Renderer/Backends/Vulkan/VulkanBackend.h"
// #ifdef DC_PLATFORM_MACOS
// #include "Renderer/Backends/Metal/MetalBackend.h"
// #endif

namespace DC
{
	std::unique_ptr<IRenderBackend> IRenderBackend::Create(BackendType type)
	{
		switch (type)
		{
		case BackendType::None:
			DC_CORE_WARN("Backend type: None. Running headless");
			return nullptr;
		case BackendType::DX11:
#ifdef DC_PLATFORM_WINDOWS
			DC_CORE_WARN("DX11 backend implemented.");
			return nullptr;
#else
			DC_CORE_ERROR("DX11 backend is Windows-only.");
			return nullptr;
#endif
		case BackendType::DX12:
#ifdef DC_PLATFORM_WINDOWS
			DC_CORE_WARN("DX12 backend implemented.");
			return nullptr;
#else
			DC_CORE_ERROR("DX12 backend is Windows-only.");
			return nullptr;
#endif
		case BackendType::Vulkan:
			DC_CORE_WARN("Vulkan backend implemented.");
			return nullptr;
		case BackendType::Metal:
#ifdef DC_PLATFORM_MACOS
			DC_CORE_WARN("Metal backend not yet implemented.");
			return nullptr;
#else
			DC_CORE_ERROR("Metal backend is macOS-only.");
			return nullptr;
#endif
		case BackendType::OpenGL:
			DC_CORE_WARN("OpenGL backend not yet implemented.");
			return nullptr;
		default:
			DC_CORE_ERROR("Unknown backend type");
			return nullptr;
		}
	}
}
