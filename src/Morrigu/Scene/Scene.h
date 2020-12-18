#ifndef MRG_CLASS_SCENE
#define MRG_CLASS_SCENE

#include "Core/Timestep.h"

#include <entt/entt.hpp>

namespace MRG
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity createEntity();

		entt::registry& reg() { return m_registry; }

		void onUpdate(Timestep ts);

	private:
		entt::registry m_registry;
	};
}  // namespace MRG

#endif
