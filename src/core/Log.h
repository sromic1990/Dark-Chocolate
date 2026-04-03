// src/core/Log.h
// Wraps spdlog in a thin static class so the rest of the enghine never directly touches spdlog API calls
// If we want to swap out spdlog for another logging library, we only have to change this file.

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace DC
{
	class Log
	{
	public:
		static void Init();
		
		// Returns shared_ptr by ref, no refcount bump on access
		static std::shared_ptr<spdlog::logger>& GetCoreLogger()
		{
			return s_CoreLogger;
		}
		static std::shared_ptr<spdlog::logger>& GetClientLogger()
		{
			return s_ClientLogger;
		}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define DC_CORE_TRACE(...) ::DC::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DC_CORE_INFO(...)  ::DC::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DC_CORE_WARN(...)  ::DC::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DC_CORE_ERROR(...) ::DC::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DC_CORE_FATAL(...) ::DC::Log::GetCoreLogger()->critical(__VA_ARGS__)


#define DC_TRACE(...) ::DC::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DC_INFO(...)  ::DC::Log::GetClientLogger()->info(__VA_ARGS__)
#define DC_WARN(...)  ::DC::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DC_ERROR(...) ::DC::Log::GetClientLogger()->error(__VA_ARGS__)

