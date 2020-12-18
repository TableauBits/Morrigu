#include "Scene.h"

#include "Renderer/Renderer2D.h"
#include "Scene/Components.h"

namespace MRG
{
	Scene::Scene() {}

	Scene::~Scene() {}

	entt::entity Scene::createEntity() { return m_registry.create(); }

	void Scene::onUpdate(Timestep)
	{
		const auto& group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (const auto& entity : group) {
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::drawQuad(glm::mat4(transform), sprite.color);
		}
	}
}  // namespace MRG
