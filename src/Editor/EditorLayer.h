// src/Editor/EditorLayer.h
#pragma once
#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include <memory>

namespace DC
{
	enum class EditorMode { Edit, Play };

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiRender() override;

		EditorMode GetMode() const { return m_Mode; }
		Scene* GetScene() const { return m_ActiveScene.get(); }


	private:
		void DrawMenuBar();
		void DrawToolbar();
		void DrawSceneHierarchy();
		void DrawProperties();
		void DrawViewport();
		void DrawStatsOverlay();
		void OnPlay();
		void OnStop();

		std::shared_ptr<Scene> m_ActiveScene;
		Entity* m_SelectedEntity			= nullptr;
		EditorMode m_Mode					= EditorMode::Edit;
		bool m_ShowDemoWindow				= false;
		float m_DeltaTime					= 0.0f;
		bool m_LayoutInitialized			= false;
	};
}