#ifndef MRG_CLASS_SCENE
#define MRG_CLASS_SCENE

#include "Core/Timestep.h"

#include <entt/entt.hpp>

#include <optional>

namespace MRG
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string{});
		void destroyEntity(Entity entity);

		void onUpdate(Timestep ts);
		void onViewportResize(uint32_t width, uint32_t height);

		[[nodiscard]] std::optional<Entity> getPrimaryCameraEntity();

	private:
		template<typename T>
		void onComponentAdded(Entity entity, T& component);

		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}  // namespace MRG

#endif
