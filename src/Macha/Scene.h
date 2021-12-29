//
// Created by Mathis Lamidey on 2021-12-25.
//

#ifndef SCENE_H
#define SCENE_H

#include "AssetRegistry.h"

#include <Morrigu.h>
#include <entt/entt.hpp>

#include <unordered_map>

class Scene
{
public:
	[[nodiscard]] MRG::EntityHandle createEntity();
	void destroyEntity(const entt::entity entityID);

	MRG::Ref<entt::registry> registry{MRG::createRef<entt::registry>()};
	std::unordered_map<entt::entity, MRG::Entity> ownedEntities{};
	entt::entity selectedEntity{entt::null};
	AssetRegistry assets;
};

#endif
