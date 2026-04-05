// src/core/Window.cpp
#include "Core/Window.h"
#include "Core/Log.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace DC
{
	static uint32_t s_GLFWWindowCount = 0;

	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::Init(const WindowProps& props)
	{
		m_data.Title = props.Title;
		m_data.Width = props.Width;
		m_data.Height = props.Height;

		if (s_GLFWWindowCount == 0)
		{
			if (!glfwInit())
			{
				throw std::runtime_error("Failed to initialize GLFW");
			}
		}

#ifdef DC_PLATFORM_MACROS
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		m_Window = glfwCreateWindow(
			static_cast<int>(m_data.Width),
			static_cast<int>(m_data.Height),
			m_data.Title.c_str(),
			nullptr, 
			nullptr
		);

		if (!m_Window) throw std::runtime_error("Failed to create GLFW window");
		glfwMakeContextCurrent(m_Window);
		++s_GLFWWindowCount;
		glfwSetWindowUserPointer(m_Window, &m_data);

		glfwSetFramebufferSizeCallback(m_Window,
			[](GLFWwindow* win, int w, int h) {
				auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(win));
				data.Width = static_cast<uint32_t>(w);
				data.Height = static_cast<uint32_t>(h);
				if (data.OnResize) data.OnResize(data.Width, data.Height);
			});


		glfwSetWindowCloseCallback(m_Window,
			[](GLFWwindow* win) {
				auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(win));
				if (data.OnClose) data.OnClose();
			});

	}

	void Window::Shutdown()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
			--s_GLFWWindowCount;
		}
		if (s_GLFWWindowCount == 0) glfwTerminate();
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window) != 0;
	}

	void Window::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}

	void* Window::GetNativeHandle() const
	{
		return static_cast<void*>(m_Window);
	}
}
