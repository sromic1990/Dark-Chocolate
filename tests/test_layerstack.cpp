// tests/test_layerstack.cpp
#include <catch2/catch_test_macros.hpp>
#include "core/Layer.h"
#include "core/LayerStack.h"
#include <vector>
#include <string>

#include "catch2/catch_approx.hpp"

// MockLayer: minimal concrete Layer that records every lifecycle call.
// Cannot test Layer directly (since it is abstract).
// MockLayer is the standard pattern for testing base classes.
class MockLayer : public DC::Layer
{
public:
	explicit MockLayer(const std::string& name) : DC::Layer(name) {}
	int attachCount = 0;
	int detachCount = 0;
	int updateCount = 0;
	int imguiCount = 0;
	float lastDelta = 0.0f;

	void OnAttach() override { ++attachCount; }
	void OnDetach() override { ++detachCount; }
	void OnUpdate(float delta) override { ++updateCount; lastDelta = delta; }
	void OnImGuiRender() override { ++imguiCount; }
};

// Helper: push a named layer, return non-owning raw pointer.
static MockLayer* pushNamed(DC::LayerStack& stack, const std::string& name)
{
	auto layer = std::make_unique<MockLayer>(name);
	auto* ptr = layer.get();
	stack.PushLayer(std::move(layer));
	return ptr;
}

// --Identity--
TEST_CASE("Stack allows layers with duplicate names", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "SameName");
	auto* b = pushNamed(stack, "SameName");

	REQUIRE(stack.Size() == 2u);

	stack.UpdateAll(0.016f);
	REQUIRE(a->updateCount == 1);
	REQUIRE(b->updateCount == 1);
}

// --Lifecycle--
TEST_CASE("PushLayer calls OnAttach once", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	REQUIRE(a->attachCount ==1);
	CHECK(a->detachCount == 0);
}

TEST_CASE("Pushing second layer does not re-attach first", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	auto* b = pushNamed(stack, "B");
	REQUIRE(a->attachCount == 1);
	REQUIRE(b->attachCount == 1);
}

TEST_CASE("PopLayer calls OnDetach once", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	auto owned = stack.PopLayer(a);
	CHECK(a->detachCount == 1);
}

TEST_CASE("PopLayer removes layer from update cycle", "[layerstack]") 
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	auto owned =  stack.PopLayer(a);
	stack.UpdateAll(0.016f);
	REQUIRE(a->updateCount == 0);
}

TEST_CASE("Pop of non-existent layer is safe no-op", "[layerstack]") 
{
	DC::LayerStack stack;
	MockLayer outsider("outside");
	REQUIRE_NOTHROW(stack.PopLayer(&outsider));
}

// --Update--
TEST_CASE("UpdateAll calls every layer with correct delta time", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	auto* b = pushNamed(stack, "B");
	stack.UpdateAll(0.033f);
	REQUIRE(a->updateCount == 1);
	REQUIRE(b->updateCount == 1);
	REQUIRE(a->lastDelta == Catch::Approx(0.033f));
	REQUIRE(b->lastDelta == Catch::Approx(0.033f));
}

TEST_CASE("Update on empty stack is safe", "[layerstack]")
{
	DC::LayerStack stack;
	REQUIRE_NOTHROW(stack.UpdateAll(0.016f));
}

TEST_CASE("UpdateAll called multiple times accumulates correctly", "[layerstack]")
{
	DC::LayerStack stack;
	auto* a = pushNamed(stack, "A");
	stack.UpdateAll(0.016f);
	stack.UpdateAll(0.016f);
	stack.UpdateAll(0.016f);
	REQUIRE(a->updateCount == 3);
}

// --Ordering--
TEST_CASE("Update order is first-pushed, first-updated", "[layerstack]")
{
	DC::LayerStack stack;
	std::vector < std::string> order;

	struct OrderLayer : DC::Layer
	{
		std::vector<std::string>& order;
		OrderLayer(const std::string& name, std::vector<std::string>& order) 
			: DC::Layer(name), order(order) {}
		void OnUpdate(float) override { order.emplace_back(GetName()); }
	};

	stack.PushLayer(std::make_unique<OrderLayer>("First", order));
	stack.PushLayer(std::make_unique<OrderLayer>("Second", order));
	stack.PushLayer(std::make_unique<OrderLayer>("Third", order));
	stack.UpdateAll(0.016f);

	REQUIRE(order.size() == 3u);
	CHECK(order[0] == "First");
	CHECK(order[1] == "Second");
	CHECK(order[2] == "Third");
}

// --Ownership--
TEST_CASE("Stack takes ownership on push", "[layerstack]")
{
	DC::LayerStack stack;
	auto owned = std::make_unique<MockLayer>("owned");
	auto* raw = owned.get();
	stack.PushLayer(std::move(owned));
	REQUIRE(owned == nullptr); // moved from, so unique pointer is null
	REQUIRE(raw != nullptr); // raw pointer is still valid
}