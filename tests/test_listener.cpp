// tests/test_listener.cpp
// Catch2 series listener: initializes the engine logger with output suppressed.
// Runs once before any TEST_CASE

#include <catch2/catch_session.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <spdlog/spdlog.h>
#include "Core/Log.h"

struct DCTestsListener : Catch::EventListenerBase
{
	using EventListenerBase::EventListenerBase;

	void testRunStarting(Catch::TestRunInfo const& testRunInfo) override
	{
		DC::Log::Init();
		// Suppress all engine log output during tests.
		// Tests should be silent unless they fail.
		DC::Log::GetCoreLogger()->set_level(spdlog::level::off);
		DC::Log::GetClientLogger()->set_level(spdlog::level::off);
	}
};

CATCH_REGISTER_LISTENER(DCTestsListener);