// src/Core/Application.cpp
#include "Core/Application.h"
#include "Core/Log.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace DC
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpec& spec) : m_Spec(spec)
	{
		if (s_Instance)
			throw std::runtime_error("Application already exists");
		s_Instance = this;

		Log::Init();
		DC_CORE_INFO("Dark Chocolate starting -- backend: {}", static_cast<int>(spec.Backend));

		m_Window = std::make_unique<Window>(WindowProps{spec.Name, spec.Width, spec.Height});

		m_Window->SetResizeCallback([this](uint32_t w, uint32_t h)
			{
				OnWindowResize(w, h);
			});
		m_Window->SetCloseCallback([this]()
			{
				OnWindowClose();
			});
		m_Window->SetVSync(spec.VSync);

		m_Backend = IRenderBackend::Create(spec.Backend);

		if (m_Backend)
		{
			if (!m_Backend->Init(m_Window->GetNativeHandle(), spec.Width, spec.Height))
				throw std::runtime_error("Backend Init() failed.");
			DC_CORE_INFO("Backend '{}' initialized.", m_Backend->GetName());
		}
		else
		{
			DC_CORE_WARN("No GPU backend, running headless.");
		}
		RenderCommand::Init(m_Backend.get());
	}

	Application::~Application()
	{
		DC_CORE_INFO("Application shutting down");
		m_LayerStack.Clear();
		if (m_Backend) m_Backend->Shutdown();
		s_Instance = nullptr;
	}

	void Application::Run()
	{
		m_Running = true;
		OnInit();
		DC_CORE_INFO("Entering main loop.");

		while (m_Running && !m_Window->ShouldClose())
		{
			float time = static_cast<float>(glfwGetTime());
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
				m_LayerStack.UpdateAll(deltaTime);

			if (!m_Minimized)
			{
				RenderCommand::BeginFrame({ 0.1f, 0.1f, 0.15f, 1.0f });
				m_LayerStack.RenderImGuiAll();
				RenderCommand::EndFrame();
				RenderCommand::Present(m_Spec.VSync);
			}
			m_Window->OnUpdate();
		}
		OnShutdown();
	}

	void Application::PushLayer(std::unique_ptr<Layer> layer)
	{
		m_LayerStack.PushLayer(std::move(layer));
	}

	void Application::OnWindowResize(uint32_t w, uint32_t h)
	{
		if (w == 0 || h == 0)
		{
			m_Minimized = true;
			return;
		}
		m_Minimized = false;
		RenderCommand::OnResize(w, h);
	}

	void Application::OnWindowClose()
	{
		m_Running = false;
	}
}
