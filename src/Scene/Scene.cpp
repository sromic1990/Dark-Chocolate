// src/Scene/Scene.cpp
#include "Scene/Scene.h"

namespace DC
{
	Entity& Scene::CreateEntity(const std::string& name)
	{
		m_Entities.push_back(std::make_unique<Entity>(name));
		m_Modified = true;
		return *m_Entities.back();
	}

	void Scene::Clear()
	{
		m_Entities.clear();
		m_Modified = true;
	}
}
