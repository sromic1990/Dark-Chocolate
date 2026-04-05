// src/Editor/EditorApp.cpp
#include "core/Application.h"
#include "editor/EditorLayer.h"

namespace DC
{
	class DarkChocolateEditor : public Application
	{
	public:
		explicit DarkChocolateEditor(const ApplicationSpec& spec) : Application(spec)
		{}

		void OnInit() override
		{
			PushLayer(std::make_unique<EditorLayer>());
		}
		
	};


	Application* CreateApplication()
	{
		ApplicationSpec spec;
		spec.Name		= "Dark Chocolate Editor";
		spec.Width		= 1600;
		spec.Height		= 900;
		spec.VSync		= true;

		// All backends are stubs for now.
#if defined(DC_PLATFORM_WINDOWS)
		spec.Backend = BackendType::DX11;
#elif defined(DC_PLATFORM_MACOS)
		spec.Backend = BackendType::Metal;
#else
		spec.Backend = BackendType::Vulkan;
#endif


		return new DarkChocolateEditor(spec);
	}
}
