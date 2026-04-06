// tests/test_log.cpp
#include <catch2/catch_test_macros.hpp>
#include "Core/Log.h"

// After the session listener runs, Init() has already been called.
// Calling it again must be safe (spdlog returns existing loggers).
TEST_CASE("Log::Init is idempotent", "[log]")
{
	REQUIRE_NOTHROW(DC::Log::Init());
}

TEST_CASE("Core logger is non-null after Init", "[log]")
{
	DC::Log::Init();
	REQUIRE(DC::Log::GetCoreLogger() != nullptr);
}

TEST_CASE("Client logger is non-null after Init", "[log]")
{
	DC::Log::Init();
	REQUIRE(DC::Log::GetClientLogger() != nullptr);
}

TEST_CASE("Log Macros compile and run without crash", "[log]")
{
	DC::Log::Init();
	REQUIRE_NOTHROW(DC_CORE_INFO("Test info message"));
	REQUIRE_NOTHROW(DC_CORE_WARN("Test warning message"));
	REQUIRE_NOTHROW(DC_CORE_ERROR("Test error message"));
}