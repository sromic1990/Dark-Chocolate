// src/Core/EntryPoint.cpp
#include "Core/Application.h"
#include "Core/Log.h"

extern DC::Application* DC::CreateApplication();

int main()
{
	try
	{
		auto* app = DC::CreateApplication();
		app->Run();
		delete app;
	}
	catch (const std::exception& e)
	{
		DC_CORE_FATAL("Fatal exception {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}