#ifndef MRG_CLASS_SCENE
#define MRG_CLASS_SCENE

#include "Core/Timestep.h"
#include "Renderer/EditorCamera.h"

#include <entt/entity/registry.hpp>

#include <optional>

namespace MRG
{
	class Entity;

	class Scene
	{
	public:
		Entity createEntity(const std::string& name = std::string{});
		void destroyEntity(Entity entity);

		void onUpdate(Timestep ts);
		void onEditorUpdate(Timestep ts, EditorCamera& camera);
		void onViewportResize(uint32_t width, uint32_t height);

		[[nodiscard]] uint32_t objectIDAt(uint32_t x, uint32_t y);

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
