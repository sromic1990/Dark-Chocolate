// tests/test_scene.cpp
#include <catch2/catch_test_macros.hpp>
#include "Scene/Scene.h"

TEST_CASE("New scene is empty", "[scene]")
{
	DC::Scene scene;
	REQUIRE(scene.GetEntities().empty());
}

TEST_CASE("CreateEntity adds to scene", "[scene]")
{
	DC::Scene scene;
	scene.CreateEntity("E");
	REQUIRE(scene.GetEntities().size() == 1u);
}

TEST_CASE("CreateEntity returns correct entity", "[scene]")
{
	DC::Scene scene;
	auto& e = scene.CreateEntity("Hero");
	REQUIRE(e.GetComponent<DC::TagComponent>().Tag == "Hero");
}

TEST_CASE("Multiple entities grow scene", "[scene]")
{
	DC::Scene scene;
	scene.CreateEntity("A");
	scene.CreateEntity("B");
	scene.CreateEntity("C");
	REQUIRE(scene.GetEntities().size() == 3u);
}

TEST_CASE("Clear removes all entities", "[scene]")
{
	DC::Scene scene;
	scene.CreateEntity("A");
	scene.CreateEntity("B");
	scene.CreateEntity("C");
	scene.Clear();
	REQUIRE(scene.GetEntities().empty());
}

TEST_CASE("Scene is not modified initially", "[scene]")
{
	DC::Scene scene;
	REQUIRE_FALSE(scene.IsModified());
}

TEST_CASE("CreateEntity marks scene modified", "[scene]")
{
	DC::Scene scene;
	scene.CreateEntity("A");
	REQUIRE(scene.IsModified());
}

TEST_CASE("SetModified false clears flag", "[scene]")
{
	DC::Scene scene;
	scene.CreateEntity("A");
	scene.SetModified(false);
	REQUIRE_FALSE(scene.IsModified());
}

TEST_CASE("Default scene name is Untitled Scene", "[scene]")
{
	DC::Scene scene;
	REQUIRE(scene.GetName() == "Untitled Scene");
}

TEST_CASE("SetName changes name and sets modified", "[scene]")
{
	DC::Scene scene;
	scene.SetName("Level 1");
	REQUIRE(scene.GetName() == "Level 1");
	REQUIRE(scene.IsModified());
}