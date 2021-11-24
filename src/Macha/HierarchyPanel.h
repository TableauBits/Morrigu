//
// Created by Mathis Lamidey on 2021-11-22
//

#ifndef HIERARCHY_PANEL_H
#define HIERARCHY_PANEL_H

#include <Morrigu.h>

class HierarchyPanel
{
public:
	void onImGuiUpdate(const std::vector<MRG::Ref<MRG::Entity>>& entities);

	MRG::Ref<MRG::Entity> selectedEntity{};

private:
	void renderEntity(const MRG::Ref<MRG::Entity>& entity);
	void modifySelectedEntity(const MRG::Ref<MRG::Entity>& entity);
};

#endif
