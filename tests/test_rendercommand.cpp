// tests/test_rendercommand.cpp
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <renderer/IRenderBackend.h>
#include <renderer/RenderCommand.h>

// SpyBackend: records every call so tests can verify forwarding.
// No mocking framework needed. Each counter starts at 0.
class SpyBackend : public DC::IRenderBackend
{
public:
	int beginFrameCount = 0;
	int endFrameCount = 0;
	int presentCount = 0;
	int onResizeCount = 0;
	int drawTriangleCount = 0;
	bool lastVsync = false;
	uint32_t lastResizeW = 0;
	uint32_t lastResizeH = 0;
	DC::ClearColor lastClearColor{};

	bool Init(void*, uint32_t, uint32_t) override { return true; }
	void Shutdown() override {}
	void BeginFrame(const DC::ClearColor& clearColor) override { ++beginFrameCount; lastClearColor = clearColor; }
	void EndFrame() override { ++endFrameCount; }
	void Present(bool vSync) override { ++presentCount; lastVsync = vSync; }
	void OnResize(uint32_t w, uint32_t h) override { ++onResizeCount; lastResizeH = h; lastResizeW = w; }
	std::string GetName() const override { return "SpyBackend"; }
	void DrawTriangle() override { ++drawTriangleCount; }
};

// Helper fixture: initialize RenderCommand with a fresh SpyBackend.
struct RenderCommandFixture
{
	SpyBackend spy;
	RenderCommandFixture() { DC::RenderCommand::Init(&spy); }
	~RenderCommandFixture() { DC::RenderCommand::Init(nullptr); } // cleanup
};

TEST_CASE("BeginFrame forwards to backend", "[rendercommand]")
{
	RenderCommandFixture f;
	DC::ClearColor cc{0.1f, 0.2f, 0.3f, 1.0f};
	DC::RenderCommand::BeginFrame(cc);
	REQUIRE(f.spy.beginFrameCount == 1);
	REQUIRE(f.spy.lastClearColor.r == Catch::Approx(0.1f));
}

TEST_CASE("EndFrame forwards to backend", "[rendercommand]")
{
	RenderCommandFixture f;
	DC::RenderCommand::EndFrame();
	REQUIRE(f.spy.endFrameCount == 1);
}

TEST_CASE("Present forwards vsync flag", "[rendercommand]")
{
	RenderCommandFixture f;
	DC::RenderCommand::Present(true);
	REQUIRE(f.spy.presentCount == 1);
	REQUIRE(f.spy.lastVsync == true);

	DC::RenderCommand::Present(false);
	REQUIRE(f.spy.presentCount == 2);
	REQUIRE(f.spy.lastVsync == false);
}

TEST_CASE("OnResize forwards dimensions", "[rendercommand]")
{
	RenderCommandFixture f;
	DC::RenderCommand::OnResize(1920, 1080);
	REQUIRE(f.spy.onResizeCount == 1);
	REQUIRE(f.spy.lastResizeW == 1920u);
	REQUIRE(f.spy.lastResizeH == 1080u);
}

TEST_CASE("GetBackendName delegates to backend", "[rendercommand]")
{
	RenderCommandFixture f;
	REQUIRE(DC::RenderCommand::GetBackendName() == "SpyBackend");
}

TEST_CASE("DrawTriangle delegates to backend", "[rendercommand]")
{
	RenderCommandFixture f;
	DC::RenderCommand::DrawTriangle();
	REQUIRE(f.spy.drawTriangleCount == 1);
}

TEST_CASE("Null backend: all calls are safe no-ops", "[rendercommand]") 
{
	// Save current backend so it can be restored after this test.
	// Use a local fixture to ensure cleanup even on test failure.
	RenderCommandFixture guard;
	DC::RenderCommand::Init(nullptr);
	REQUIRE_NOTHROW(DC::RenderCommand::BeginFrame({}));
	REQUIRE_NOTHROW(DC::RenderCommand::EndFrame());
	REQUIRE_NOTHROW(DC::RenderCommand::Present(true));
	REQUIRE_NOTHROW(DC::RenderCommand::OnResize(800, 600));
	REQUIRE_NOTHROW(DC::RenderCommand::DrawTriangle());
	REQUIRE(DC::RenderCommand::GetBackendName() == "None");
}

TEST_CASE("BackendType enum values are distinct", "[rendercommand]") 
{
	REQUIRE(DC::BackendType::None != DC::BackendType::DX11);
	REQUIRE(DC::BackendType::DX11 != DC::BackendType::DX12);
	REQUIRE(DC::BackendType::DX12 != DC::BackendType::Vulkan);
	REQUIRE(DC::BackendType::Vulkan != DC::BackendType::Metal);
	REQUIRE(DC::BackendType::Metal != DC::BackendType::OpenGL);
}

