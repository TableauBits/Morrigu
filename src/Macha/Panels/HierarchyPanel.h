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
	void onImGuiUpdate(entt::registry& registry, const entt::entity selectedEntity);

	struct Callbacks
	{
		std::function<void()> entityCreation;
		std::function<void(const entt::entity)> entityDestruction;
		std::function<void(const entt::entity)> entitySelected;
	} callbacks;

private:
	std::optional<entt::entity> m_entityToDelete{};

	void renderEntity(entt::registry& registry, const entt::entity entity, bool isSelected);
	void modifySelectedEntity(const entt::entity entity);
};

#endif
