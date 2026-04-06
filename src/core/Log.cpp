// src/Core/Log.cpp
#include "Log.h"

namespace DC
{
	// Static member definitions
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
        // spdlog::stdout_color_mt creates a multi-threaded console logger.
        // Init() is safe to call multiple times.
        spdlog::set_pattern("%^[%T] [%l] %n: %v%$");
        if (!spdlog::get("CORE"))
            s_CoreLogger = spdlog::stdout_color_mt("CORE");
        else
            s_CoreLogger = spdlog::get("CORE");
        s_CoreLogger->set_level(spdlog::level::trace);


        if (!spdlog::get("APP"))
            s_ClientLogger = spdlog::stdout_color_mt("APP");
        else
            s_ClientLogger = spdlog::get("APP");
        s_ClientLogger->set_level(spdlog::level::trace);


        s_CoreLogger->info("Logger initialised.");
	}
}