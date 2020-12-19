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
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		{
			auto view = m_registry.view<TransformComponent, CameraComponent>();

			for (const auto& entity : view) {
				const auto& [transformComp, cameraComp] = view.get<TransformComponent, CameraComponent>(entity);
				if (cameraComp.primary) {
					mainCamera = &cameraComp.camera;
					cameraTransform = &transformComp.transform;
					break;
				}
			}

			if (mainCamera) {
				Renderer2D::beginScene(mainCamera->getProjection(), *cameraTransform);

				const auto& group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (const auto& entity : group) {
					const auto& [transformComp, spriteComp] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::drawQuad(transformComp.transform, spriteComp.color);
				}

				Renderer2D::endScene();
			}
		}
	}
}  // namespace MRG
