#ifndef MRG_CLASS_ENTITY
#define MRG_CLASS_ENTITY

#include "Core/Core.h"
#include "Scene/Scene.h"

#include <entt/entt.hpp>

namespace MRG
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			MRG_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
			auto& component = m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
			m_scene->onComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T>
		[[nodiscard]] T& getComponent()
		{
			MRG_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			return m_scene->m_registry.get<T>(m_handle);
		}

		template<typename T>
		[[nodiscard]] bool hasComponent()
		{
			return m_scene->m_registry.has<T>(m_handle);
		}

		template<typename T>
		void removeComponent()
		{
			MRG_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			m_scene->m_registry.remove<T>(m_handle);
		}

		operator bool() const { return m_handle != entt::null; };
		operator entt::entity() const { return m_handle; };
		explicit operator uint32_t() const { return static_cast<uint32_t>(m_handle); }
		[[nodiscard]] bool operator==(const Entity& other) const { return m_handle == other.m_handle && m_scene == other.m_scene; }
		[[nodiscard]] bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_handle{entt::null};
		Scene* m_scene = nullptr;
	};
}  // namespace MRG

#endif
