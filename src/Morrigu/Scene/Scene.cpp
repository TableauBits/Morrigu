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
		m_registry.view<NativeScriptComponent>().each([=](const auto entity, NativeScriptComponent& nsc) {
			// Creating entity if we haven't encountered them before
			if (!nsc.instance) {
				nsc.instance = nsc.instanciateScript();
				nsc.instance->m_entity = Entity{entity, this};
				nsc.instance->onCreate();
			}

			nsc.instance->onUpdate(ts);
		});

		// 2D rendering
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		auto view = m_registry.view<TransformComponent, CameraComponent>();

		for (const auto& entity : view) {
			const auto [tc, cc] = view.get<TransformComponent, CameraComponent>(entity);
			if (cc.primary) {
				mainCamera = &cc.camera;
				cameraTransform = &tc.transform;
				break;
			}
		}

		if (mainCamera) {
			Renderer2D::beginScene(*mainCamera, *cameraTransform);

			const auto& group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (const auto& entity : group) {
				const auto [tc, sc] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawQuad(tc.transform, sc.color);
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
			auto& cc = view.get<CameraComponent>(entity);
			if (!cc.fixedAspectRatio)
				cc.camera.setViewportSize(width, height);
		}
	}
}  // namespace MRG
