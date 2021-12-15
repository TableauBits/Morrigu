//
// Created by Mathis Lamidey on 2021-11-22
//

#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include <Morrigu.h>

class HierarchyPanel
{
public:
	void onImGuiUpdate(const std::vector<MRG::Entity>& entities);

	entt::entity selectedEntity{entt::null};

private:
	void renderEntity(const MRG::Entity& entity);
	void modifySelectedEntity(const entt::entity& entity);
};

#endif
