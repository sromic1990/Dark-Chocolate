// src/Core/Window.h
#pragma once
#include <string>
#include <functional>
#include <cstdint>

// Forward declare GLFWwindow to avoid including GLFW headers in this header.
struct GLFWwindow;

namespace DC
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		WindowProps(const std::string& title = "Dark Chocolate", 
			uint32_t width = 1600, 
			uint32_t height = 900)
			: Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		using ResizeCallback = std::function<void(uint32_t w, uint32_t h)>;
		using CloseCallback = std::function<void()>;

		explicit Window(const WindowProps& props = WindowProps{});
		~Window();

		Window(const Window&)				= delete;
		Window& operator=(const Window&)	= delete;		
		Window(Window&&)					= delete;
		Window& operator=(Window&&)			= delete;

		void OnUpdate();
		uint32_t GetWidth() const { return m_data.Width; }
		uint32_t GetHeight() const { return m_data.Height; }
		void* GetNativeHandle() const;
		void SetResizeCallback(ResizeCallback cb) { m_data.OnResize = std::move(cb); }
		void SetCloseCallback(CloseCallback cb) { m_data.OnClose = std::move(cb); }
		bool ShouldClose() const;
		void SetVSync(bool enabled);

	private:
		void Init(const WindowProps& props);
		void Shutdown();

		GLFWwindow* m_Window = nullptr;

		// WindowData is stored separately because GLFW C callbacks receive only a GLFWwindow*.
		// We store a pointer to this struct as the GLFW "user pointer", so callbacks can reach our C++ state 
		// without exposing the full Window object.
		// Minimum necessary data only.
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;
			ResizeCallback OnResize;
			CloseCallback OnClose;
		};
		WindowData m_data;
	};
}
