// src/Editor/EditorLayer.cpp
#include "editor/EditorLayer.h"
#include "core/Application.h"
#include "renderer/RenderCommand.h"
#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace DC
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"),
		m_ActiveScene(std::make_shared<Scene>())
	{}

	void EditorLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 4.0f;
		style.FrameRounding = 3.0f;
		style.Colors[ImGuiCol_WindowBg]			= ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_Button]			= ImVec4(0.20f, 0.45f, 0.80f, 0.60f);
		style.Colors[ImGuiCol_TitleBgActive]	= ImVec4(0.08f, 0.08f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_TabActive]		= ImVec4(0.20f, 0.50f, 0.90f, 1.00f);

		// TEMPORARY:imgui_impl_opengl3 will be replaced per-backend.
		// EditorLayer structure stays constant. Only the next 3 statements change.
		auto* win = static_cast<GLFWwindow*>(
			Application::Get().GetWindow().GetNativeHandle());
		ImGui_ImplGlfw_InitForOpenGL(win, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		// Starter scene
		m_ActiveScene->SetName("Sample Scene");
		m_ActiveScene->SetModified(false);
		auto& cam = m_ActiveScene->CreateEntity("Main Camera");
		auto& light = m_ActiveScene->CreateEntity("Directional Light");
		auto& cube = m_ActiveScene->CreateEntity("Cube");
		cube.GetComponent<DC::TransformComponent>().Position = { 0.0f, 0.0f, -5.0f };
		light.GetComponent<TransformComponent>().Rotation = {45.0f, -45.0f, 0.0f};
		(void)cam; // this will be used later when an actual camera component is implemented.
		
	}

	void EditorLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_DeltaTime = deltaTime;
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// Full-screen DockSpace: the invisible table the editor layout sits on.
		// Without it, ImGuiConfigFlags_DockingEnable has no visible effect.
		{
			ImGuiWindowFlags flags =
				ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			const ImGuiViewport* vp = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(vp->WorkPos);
			ImGui::SetNextWindowSize(vp->WorkSize);
			ImGui::SetNextWindowViewport(vp->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("DockSpace", nullptr, flags);
			ImGui::PopStyleVar();
			ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0, 0),
				ImGuiDockNodeFlags_PassthruCentralNode);
			DrawMenuBar();
			ImGui::End();
		}
		DrawToolbar(); DrawSceneHierarchy(); DrawProperties();
		DrawViewport(); DrawStatsOverlay();
		if (m_ShowDemoWindow) ImGui::ShowDemoWindow(&m_ShowDemoWindow);


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// Multi-viewport: save/restore GL context around platform window render.
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			GLFWwindow* backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}


	}

	void EditorLayer::DrawMenuBar()
	{
		if (!ImGui::BeginMenuBar()) return;
		if (ImGui::BeginMenu("File")) 
		{
			if (ImGui::MenuItem("New Scene", "Ctrl+N")) 
			{
				m_ActiveScene = std::make_shared<Scene>(); m_SelectedEntity = nullptr;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4")) Application::Get().RequestExit();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) 
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowDemoWindow);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();


	}

	void EditorLayer::DrawToolbar()
	{
		ImGui::Begin("##Toolbar", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
		float sz = ImGui::GetWindowHeight() - 8.0f;
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - sz * 2.5f) / 2.0f);
		ImGui::PushStyleColor(ImGuiCol_Button,
			m_Mode == EditorMode::Play
			? ImVec4(0.1f, 0.7f, 0.1f, 1.0f)
			: ImVec4(0.2f, 0.5f, 0.2f, 0.8f));
		if (ImGui::Button(m_Mode == EditorMode::Edit ? " > " : " || ", ImVec2(sz, sz)))
			m_Mode == EditorMode::Edit ? OnPlay() : OnStop();
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 0.8f));
		if (ImGui::Button(" [] ", ImVec2(sz, sz)) && m_Mode == EditorMode::Play) OnStop();
		ImGui::PopStyleColor();
		ImGui::End();


	}

	void EditorLayer::DrawSceneHierarchy()
	{
		ImGui::Begin("Scene Hierarchy");
		for (auto& entity : m_ActiveScene->GetEntities()) 
		{
			const auto& name = entity->GetComponent<TagComponent>().Tag;
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_SelectedEntity == entity.get()) flags |= ImGuiTreeNodeFlags_Selected;
			bool open = ImGui::TreeNodeEx(name.c_str(), flags);
			if (ImGui::IsItemClicked()) m_SelectedEntity = entity.get();
			if (open) ImGui::TreePop();
		}
		if (ImGui::BeginPopupContextWindow("##HCtx", ImGuiPopupFlags_MouseButtonRight)) 
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_ActiveScene->CreateEntity("New Entity");
			ImGui::EndPopup();
		}
		ImGui::End();


	}

	void EditorLayer::DrawProperties()
	{
		ImGui::Begin("Properties");
		if (m_SelectedEntity && m_SelectedEntity->HasComponent<TagComponent>()) 
		{
			auto& tag = m_SelectedEntity->GetComponent<TagComponent>();
			char buf[256];
			strncpy(buf, tag.Tag.c_str(), sizeof(buf) - 1);
			buf[sizeof(buf) - 1] = '\0';
			if (ImGui::InputText("##Name", buf, sizeof(buf))) tag.Tag = buf;
			ImGui::Separator();
			if (m_SelectedEntity->HasComponent<TransformComponent>()) 
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) 
				{
					auto& tc = m_SelectedEntity->GetComponent<TransformComponent>();
					ImGui::DragFloat3("Position", &tc.Position.x, 0.1f);
					ImGui::DragFloat3("Rotation", &tc.Rotation.x, 0.5f);
					ImGui::DragFloat3("Scale", &tc.Scale.x, 0.1f, 0.01f);
				}
			}
		}
		else { ImGui::TextDisabled("No entity selected."); }
		ImGui::End();


	}

	void EditorLayer::DrawViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		ImGui::PopStyleVar();
		ImGui::Text("GPU viewport renders here later.");
		ImGui::TextDisabled("Backend: %s", RenderCommand::GetBackendName().c_str());
		if (m_Mode == EditorMode::Play)
			ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), " PLAY MODE ");
		ImGui::End();


	}

	void EditorLayer::DrawStatsOverlay()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav;
		ImGui::SetNextWindowBgAlpha(0.70f);
		if (ImGui::Begin("##Stats", nullptr, flags)) 
		{
			ImGui::Text("FPS: %.1f (%.3f ms)",
				m_DeltaTime > 0.0f ? 1.0f / m_DeltaTime : 0.0f, m_DeltaTime * 1000.0f);
			ImGui::Text("Mode: %s", m_Mode == EditorMode::Edit ? "Edit" : "Play");
			ImGui::Text("Backend: %s", RenderCommand::GetBackendName().c_str());
		}
		ImGui::End();


	}

	void EditorLayer::OnPlay()
	{
		m_Mode = EditorMode::Play;
	}

	void EditorLayer::OnStop()
	{
		m_Mode = EditorMode::Edit;
	}
}
