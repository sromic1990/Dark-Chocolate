// tests/test_application.cpp
#include <catch2/catch_test_macros.hpp>
#include "Core/Application.h"

// ApplicationSpec defaults must watch documented values.
TEST_CASE("ApplicationSpec default values", "[application]")
{
	DC::ApplicationSpec spec;
	REQUIRE(spec.Width == 1600u);
	REQUIRE(spec.Height == 900u);
	REQUIRE(spec.VSync == true);
	REQUIRE(spec.Backend == DC::BackendType::None);	
}

// BackendType enum class must be strongly typed (no implicit int).
// The factory switch uses these integer values.
// Changing them would break serialisation of saved scenes.
TEST_CASE("BackendType enum values are stable", "[application]")
{
	REQUIRE(static_cast<int>(DC::BackendType::None) == 0);
	REQUIRE(static_cast<int>(DC::BackendType::DX11) == 1);
	REQUIRE(static_cast<int>(DC::BackendType::DX12) == 2);
	REQUIRE(static_cast<int>(DC::BackendType::Vulkan) == 3);
	REQUIRE(static_cast<int>(DC::BackendType::Metal) == 4);
	REQUIRE(static_cast<int>(DC::BackendType::OpenGL) == 5);
}