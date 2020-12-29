#include "Entity.h"

namespace MRG
{
	Entity::Entity(entt::entity handle, Scene* scene) : m_handle(handle), m_scene(scene) {}
}  // namespace MRG
