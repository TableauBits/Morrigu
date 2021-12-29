//
// Created by Mathis Lamidey on 2021-12-25.
//

#include "Scene.h"

#include "Components/EntitySettings.h"

MRG::EntityHandle Scene::createEntity()
{
	auto entity = MRG::Entity{registry};
	entity.addComponent<Components::EntitySettings>();
	const auto handle = MRG::EntityHandle{entity};

	ownedEntities.insert({entity.getID(), std::move(entity)});

	return handle;
}

void Scene::destroyEntity(const entt::entity entityID) { ownedEntities.erase(entityID); }
