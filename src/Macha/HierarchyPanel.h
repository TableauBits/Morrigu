//
// Created by Mathis Lamidey on 2021-11-22
//

#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include <Morrigu.h>

#include <functional>

class HierarchyPanel
{
public:
	void onImGuiUpdate(const std::vector<MRG::Entity>& entities);

	entt::entity selectedEntity{entt::null};

	struct Callbacks
	{
		std::function<void(const entt::entity)> entitySelected;
		std::function<void()> entityCreation;
	} callbacks;

private:
	void renderEntity(const MRG::Entity& entity);
	void modifySelectedEntity(const entt::entity& entity);
};

#endif
