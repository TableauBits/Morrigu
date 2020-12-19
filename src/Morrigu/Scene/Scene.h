#ifndef MRG_CLASS_SCENE
#define MRG_CLASS_SCENE

#include "Core/Timestep.h"

#include <entt/entt.hpp>

namespace MRG
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string{});

		void onUpdate(Timestep ts);

	private:
		entt::registry m_registry;

		friend class Entity;
	};
}  // namespace MRG

#endif
