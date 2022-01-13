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

void Scene::onUpdate(MRG::Timestep, MRG::Renderer& renderer)
{
	// TODO: update scripts here

	Lights lightsInfo{};
	// update directional light info
	const auto dlView   = registry->view<Components::DirectionalLight>();
	const auto dlEntity = dlView.front();
	if (registry->valid(dlEntity)) {
		const auto [dlc]       = dlView.get(dlEntity);
		lightsInfo.directional = dlc;
	}

	// update point lights info
	const auto plView = registry->view<Components::PointLight>();
	auto plCount      = 0;
	for (const auto [_, plc] : plView.each()) {
		if (plCount >= POINT_LIGHT_COUNT) { break; }

		lightsInfo.pointLights[plCount] = plc;
		++plCount;
	}

	lightsInfo.activePointLightCount = plCount;

	void* data;
	vmaMapMemory(renderer.getAllocator(), m_lightsInfoBuffer.allocation, &data);
	memcpy(data, &lightsInfo, sizeof(lightsInfo));
	vmaUnmapMemory(renderer.getAllocator(), m_lightsInfoBuffer.allocation);
}
