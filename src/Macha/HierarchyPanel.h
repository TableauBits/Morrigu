//
// Created by Mathis Lamidey on 2021-11-22
//

#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include <Morrigu.h>

#include <functional>
#include <map>
#include <optional>

class HierarchyPanel
{
public:
	void onImGuiUpdate(const std::unordered_map<entt::entity, MRG::Entity>& entities);

	entt::entity selectedEntity{entt::null};

	struct Callbacks
	{
		std::function<void()> entityCreation;
		std::function<void(const entt::entity)> entityDestruction;
		std::function<void(const entt::entity)> entitySelected;
	} callbacks;

private:
	std::optional<entt::entity> m_entityToDelete{};

	void renderEntity(const MRG::Entity& entity);
	void modifySelectedEntity(const entt::entity& entity);
};

#endif
