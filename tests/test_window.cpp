// tests/test_window.cpp
#include <catch2/catch_test_macros.hpp>
#include "Core/Window.h"

TEST_CASE("WindowProps default values", "[window]")
{
	DC::WindowProps props;
	REQUIRE(props.Title == "Dark Chocolate");
	REQUIRE(props.Width == 1600u);
	REQUIRE(props.Height == 900u);
}

TEST_CASE("WindowProps custom values", "[window]")
{
	DC::WindowProps props("My App", 800u, 600u);
	REQUIRE(props.Title == "My App");
	REQUIRE(props.Width == 800u);
	REQUIRE(props.Height == 600u);
}