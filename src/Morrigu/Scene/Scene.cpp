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

	void Scene::onUpdate(Timestep ts)
	{
		// Updating scripts
		m_registry.view<NativeScriptComponent>().each([=](const auto entity, NativeScriptComponent& nativeScriptComp) {
			// Creating entity if we haven't encountered them before
			if (!nativeScriptComp.instance) {
				nativeScriptComp.instantiateFunction();
				nativeScriptComp.instance->m_entity = Entity{entity, this};

				if (nativeScriptComp.onCreateFunction)
					nativeScriptComp.onCreateFunction(nativeScriptComp.instance);
			}

			if (&nativeScriptComp.onUpdateFunction)
				nativeScriptComp.onUpdateFunction(nativeScriptComp.instance, ts);
		});

		// 2D rendering
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
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

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto view = m_registry.view<CameraComponent>();
		for (const auto& entity : view) {
			auto& cameraComp = view.get<CameraComponent>(entity);
			if (!cameraComp.fixedAspectRatio)
				cameraComp.camera.setViewportSize(width, height);
		}
	}
}  // namespace MRG
