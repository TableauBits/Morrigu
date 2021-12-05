//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_ENTITY_H
#define MORRIGU_ENTITY_H

#include "Entity/Components/Tag.h"
#include "Entity/Components/Transform.h"

#include <entt/entt.hpp>

#include <iterator>

namespace MRG
{
	template<typename Component>
	concept NonEssentialComponent = !std::is_same_v<Component, Components::Transform> && !std::is_same_v<Component, Components::Tag>;

	class Entity
	{
	public:
		Entity(const Ref<entt::registry>& registryRef) : m_registry{registryRef}
		{
			m_id = m_registry->create();
			m_registry->emplace<Components::Transform>(m_id);
			m_registry->emplace<Components::Tag>(m_id, fmt::format("%d", m_id));
		}

		~Entity() { m_registry->destroy(m_id); }

		template<typename... ComponentTypes>
		[[nodiscard]] bool hasComponents() const
		{
			return m_registry->all_of<ComponentTypes...>(m_id);
		}

		template<typename ComponentType>
		[[nodiscard]] ComponentType& getComponent()
		{
			MRG_ENGINE_ASSERT(hasComponents<ComponentType>(), "Entity {}, does not have a component of the requested type!", m_id)
			return m_registry->get<ComponentType>(m_id);
		}
		template<typename ComponentType>
		[[nodiscard]] ComponentType& getComponent() const
		{
			MRG_ENGINE_ASSERT(hasComponents<ComponentType>(), "Entity {}, does not have a component of the requested type!", m_id)
			return m_registry->get<ComponentType>(m_id);
		}

		template<NonEssentialComponent ComponentType, typename... Args>
		ComponentType& addComponent(Args&&... args)
		{
			MRG_ENGINE_ASSERT(!hasComponents<ComponentType>(), "Entity {} already has component!", m_id)
			auto& component = m_registry->emplace<ComponentType>(m_id, std::forward<Args>(args)...);

			return component;
		}

		template<NonEssentialComponent ComponentType>
		void removeComponent()
		{
			MRG_ENGINE_ASSERT(hasComponents<ComponentType>(), "Entity {}, does not have a component of the requested type!", m_id)
			m_registry->remove<ComponentType>(m_id);
		}

		[[nodiscard]] uint32_t getID() const { return static_cast<uint32_t>(m_id); }
		[[nodiscard]] entt::entity getHandle() const { return m_id; }
		[[nodiscard]] std::string getName() const { return getComponent<Components::Tag>().tag; }

		void setName(const std::string& name) { getComponent<Components::Tag>().tag = name; }

	private:
		entt::entity m_id;
		Ref<entt::registry> m_registry;
	};

	// clang-format off
	template<typename Iterator>
	concept EntityIterator =
		std::input_iterator<Iterator> &&
		requires(Iterator it) {
			{*it} -> std::convertible_to<Ref<Entity>>;
		}
	;
	// clang-format on

}  // namespace MRG

#endif  // MORRIGU_ENTITY_H