#include "Scene.h"

#include "Renderer/Renderer2D.h"
#include "Scene/Components.h"

namespace MRG
{
	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = {m_registry.create(), this};
		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>(name.empty() ? "Entity" : name);
		return entity;
	}

	void Scene::destroyEntity(const Entity entity) { m_registry.destroy(static_cast<entt::entity>(entity)); }

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

		auto mainCamera = getPrimaryCameraEntity();

		if (mainCamera) {
			if (!mainCamera.value().hasComponent<TransformComponent>()) {
				MRG_ENGINE_ERROR("Primary camera doesn't have a tranform component!")
			}
			Renderer2D::beginScene(mainCamera.value().getComponent<CameraComponent>().camera,
			                       mainCamera.value().getComponent<TransformComponent>().getTransform());

			const auto& group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (const auto& entity : group) {
				const auto [tc, sc] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawQuad(tc.getTransform(), sc.color, static_cast<uint32_t>(entity));
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onEditorUpdate(Timestep, EditorCamera& camera)
	{
		Renderer2D::beginScene(camera);

		const auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (const auto& entity : group) {
			auto [tc, src] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::drawQuad(tc.getTransform(), src.color, static_cast<uint32_t>(entity));
		}

		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto view = m_registry.view<CameraComponent>();
		for (const auto& entity : view) {
			auto& cc = view.get<CameraComponent>(entity);
			if (!cc.fixedAspectRatio) {
				cc.camera.setViewportSize(width, height);
			}
		}
	}

	std::optional<Entity> Scene::getPrimaryCameraEntity()
	{
		const auto view = m_registry.view<CameraComponent>();

		for (const auto& entity : view) {
			if (view.get<CameraComponent>(entity).primary) {
				return Entity{entity, this};
			}
		}

		return std::nullopt;
	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity, TransformComponent&)
	{}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity, CameraComponent& component)
	{
		component.camera.setViewportSize(m_viewportWidth, m_viewportHeight);
	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity, SpriteRendererComponent&)
	{}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity, TagComponent&)
	{}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity, NativeScriptComponent&)
	{}
}  // namespace MRG
