// src/Scene/Entity.h
#pragma once
#include "Scene/Components.h"
#include <unordered_map>
#include <typeindex>
#include <any>
#include <stdexcept>

namespace DC
{
	class Entity
	{
	public:
		Entity() = default;
		explicit Entity(const std::string& name)
		{
			AddComponent<TagComponent>(name);
			AddComponent<TransformComponent>();
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			m_Components[std::type_index(typeid(T))] = std::make_any<T>(std::forward<Args>(args)...);
			return GetComponent<T>();
		}

		template<typename T>
		T& GetComponent()
		{
			return std::any_cast<T&>(m_Components.at(std::type_index(typeid(T))));
		}

		template<typename T>
		const T& GetComponent() const
		{
			return std::any_cast<const T&>(m_Components.at(std::type_index(typeid(T))));
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Components.count(std::type_index(typeid(T))) > 0;
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Components.erase(std::type_index(typeid(T)));
		}

		const std::string& GetName() const
		{
			return GetComponent<TagComponent>().Tag;
		}

	private:
		std::unordered_map<std::type_index, std::any> m_Components;
	};
}
