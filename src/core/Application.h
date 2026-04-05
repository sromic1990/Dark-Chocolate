// src/Application.h
#pragma once
#include "Core/Window.h"
#include "Core/LayerStack.h"
#include "Renderer/IRenderBackend.h"
#include "Renderer/RenderCommand.h"
#include <memory>
#include <string>

namespace DC
{
	struct ApplicationSpec
	{
		std::string Name		= "Dark Chocolate App";
		uint32_t Width			= 1600;
		uint32_t Height			= 900;
		bool VSync				= true;
		BackendType Backend		= BackendType::None;
	};

	class Application
	{
	public:
		explicit Application(const ApplicationSpec& spec);
		virtual ~Application();

		Application(const Application&)				= delete;
		Application& operator= (const Application&) = delete;
		Application(Application&&)					= delete;
		Application& operator= (Application&&)		= delete;

		void Run();
		virtual void OnInit()		{}
		virtual void OnShutdown()	{}

		void PushLayer(std::unique_ptr<Layer> layer);
		void RequestExit()								{ m_Running = false; }

		Window& GetWindow()								{ return *m_Window; }
		const ApplicationSpec& GetSpec() const			{ return m_Spec; }
		static Application& Get()						{ return *s_Instance; }

	private:
		void OnWindowResize(uint32_t w, uint32_t h);
		void OnWindowClose();

		ApplicationSpec						m_Spec;
		std::unique_ptr<Window>				m_Window;
		std::unique_ptr<IRenderBackend>		m_Backend;
		LayerStack							m_LayerStack;
		bool								m_Running		= false;
		bool								m_Minimized		= false;
		float								m_LastFrameTime = 0.0f;
		// Singleton instance
		static Application*					s_Instance;
	};

	Application* CreateApplication();
}
