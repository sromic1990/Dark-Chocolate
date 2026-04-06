// tests/test_entity.cpp
#include <catch2/catch_test_macros.hpp>
#include "catch2/catch_approx.hpp"
#include "Scene/Entity.h"
#include "Scene/Components.h"

TEST_CASE("Named entity has TagComponent with correct name", "[entity]")
{
	DC::Entity entity("Player");
	REQUIRE(entity.HasComponent<DC::TagComponent>());
	REQUIRE(entity.GetComponent<DC::TagComponent>().Tag == "Player");
}

TEST_CASE("Named entity has TransformComponent by default", "[entity]")
{
	DC::Entity entity("Player");
	REQUIRE(entity.HasComponent<DC::TransformComponent>());
}

TEST_CASE("AddComponent makes HasComponent return true", "[entity]")
{
	DC::Entity entity("E");
	REQUIRE_FALSE(entity.HasComponent<DC::MeshRendererComponent>());
	entity.AddComponent<DC::MeshRendererComponent>();
	REQUIRE(entity.HasComponent<DC::MeshRendererComponent>());
}

TEST_CASE("GetComponent returns live reference", "[entity]")
{
	DC::Entity entity("E");
	auto& tc = entity.GetComponent<DC::TransformComponent>();
	tc.Position = { 1.0f, 2.0f, 3.0f };
	auto& tc2 = entity.GetComponent<DC::TransformComponent>();
	REQUIRE(tc2.Position.x == Catch::Approx(1.0f));
	REQUIRE(tc2.Position.y == Catch::Approx(2.0f));
	REQUIRE(tc2.Position.z == Catch::Approx(3.0f));
}

TEST_CASE("AddComponent forwards constructor arguments", "[entity]")
{
	DC::Entity entity("E");
	entity.AddComponent<DC::TagComponent>("MyTag");
	REQUIRE(entity.GetComponent<DC::TagComponent>().Tag == "MyTag");
}

TEST_CASE("RemoveComponent makes HasComponent return false", "[entity]")
{
	DC::Entity entity("E");
	entity.AddComponent<DC::MeshRendererComponent>();
	REQUIRE(entity.HasComponent<DC::MeshRendererComponent>());
	entity.RemoveComponent<DC::MeshRendererComponent>();
	REQUIRE_FALSE(entity.HasComponent<DC::MeshRendererComponent>());
}

TEST_CASE("GetComponent throws if component is missing", "[entity]")
{
	DC::Entity entity("E");
	REQUIRE_THROWS_AS(
		entity.GetComponent<DC::MeshRendererComponent>(),
		std::out_of_range
	);
}

TEST_CASE("Multiple component type coexist independently", "[entity]")
{
	DC::Entity entity("E");
	entity.AddComponent<DC::MeshRendererComponent>().MeshPath = "cube.fbx";
	REQUIRE(entity.GetComponent<DC::TagComponent>().Tag == "E");
	REQUIRE(entity.GetComponent<DC::MeshRendererComponent>().MeshPath == "cube.fbx");
}