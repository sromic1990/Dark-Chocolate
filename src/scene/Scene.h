// src/Scene/Scene.h
#pragma once
#include "scene/Entity.h"
#include <vector>
#include <memory>
#include <string>

namespace DC
{
	class Scene
	{
	public:
		Entity& CreateEntity(const std::string& name = "Entity");
		void Clear();

		std::vector<std::unique_ptr<Entity>>& GetEntities() { return m_Entities; }
		bool IsModified() { return m_Modified; }
		void SetModified(bool m) { m_Modified = m; }
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; SetModified(true); }

	private:
		std::string m_Name = "Untitled Scene";
		std::vector<std::unique_ptr<Entity>> m_Entities;
		bool m_Modified = false;
	};
}
