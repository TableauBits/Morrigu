#include "Scene.h"

#include "Renderer/Renderer2D.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace MRG
{
	Scene::Scene() {}

	Scene::~Scene() {}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = {m_registry.create(), this};
		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>(name.empty() ? "Entity" : name);
		return entity;
	}

	void Scene::onUpdate(Timestep)
	{
		const auto& group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (const auto& entity : group) {
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::drawQuad(glm::mat4(transform), sprite.color);
		}
	}
}  // namespace MRG
