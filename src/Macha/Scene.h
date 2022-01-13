//
// Created by Mathis Lamidey on 2021-12-25.
//

#ifndef SCENE_H
#define SCENE_H

#include "AssetRegistry.h"

#include "Components/LightCasters/DirectionalLight.h"
#include "Components/LightCasters/PointLight.h"

#include <Morrigu.h>
#include <entt/entt.hpp>

#include <unordered_map>

class Scene
{
public:
	Scene(MRG::Renderer& renderer);

	[[nodiscard]] MRG::EntityHandle createEntity();
	void destroyEntity(const entt::entity entityID);

	void onUpdate(MRG::Timestep ts, MRG::Renderer& renderer);

	MRG::Ref<entt::registry> registry{MRG::createRef<entt::registry>()};
	std::unordered_map<entt::entity, MRG::Entity> ownedEntities{};
	entt::entity selectedEntity{entt::null};
	AssetRegistry assets;

private:
	static constexpr auto POINT_LIGHT_COUNT = 16;
	struct Lights
	{
		Components::DirectionalLight directional;
		Components::PointLight pointLights[POINT_LIGHT_COUNT];
		uint32_t activePointLightCount;
	};
	MRG::AllocatedBuffer m_lightsInfoBuffer{};
};

#endif
