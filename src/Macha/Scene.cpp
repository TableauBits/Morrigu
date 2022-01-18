//
// Created by Mathis Lamidey on 2021-12-25.
//

#include "Scene.h"

#include "Components/EntitySettings.h"

Scene::Scene(MRG::Renderer& renderer)
    : m_lightsInfoBuffer{
        std::move(renderer.createBuffer(sizeof(Scene::Lights), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU))}
{
	renderer.bindBufferToSet1(m_lightsInfoBuffer, sizeof(Scene::Lights));
}

MRG::EntityHandle Scene::createEntity()
{
	auto entity = MRG::Entity{registry};
	entity.addComponent<Components::EntitySettings>();
	const auto handle = MRG::EntityHandle{entity};

	ownedEntities.insert({entity.getID(), std::move(entity)});

	return handle;
}

void Scene::destroyEntity(const entt::entity entityID) { ownedEntities.erase(entityID); }

void Scene::onUpdate(MRG::Timestep)
{
	// TODO: update scripts here

	Lights lightsInfo{};
	// update directional light info
	const auto dlView   = registry->view<MRG::Components::Transform, Components::DirectionalLight>();
	const auto dlEntity = dlView.front();
	if (registry->valid(dlEntity)) {
		const auto [tc, dlc]   = dlView.get(dlEntity);
		lightsInfo.directional = DirectionalLight{
		  .direction = glm::vec4{glm::rotate(glm::quat{tc.rotation}, glm::vec3{0.f, 0.f, -1.f}), 1.f},
		  .color     = dlc.color,
		};
	}

	// update point lights info
	const auto plView = registry->view<MRG::Components::Transform, Components::PointLight>();
	auto plCount      = 0;
	for (const auto [_, tc, plc] : plView.each()) {
		if (plCount >= POINT_LIGHT_COUNT) { break; }

		lightsInfo.pointLights[plCount] = PointLight{
		  .position    = glm::vec4{tc.translation, 1.f},
		  .color       = plc.color,
		  .attenuation = plc.attenuation,
		};
		++plCount;
	}
	lightsInfo.activePointLightCount = plCount;

	m_lightsInfoBuffer.uploadData(lightsInfo);
}
