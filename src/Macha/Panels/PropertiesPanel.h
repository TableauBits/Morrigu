//
// Created by Mathis Lamidey on 2021-11-23
//

#ifndef PROPERTIES_PANEL_H
#define PROPERTIES_PANEL_H

#include "AssetRegistry.h"

#include <Morrigu.h>

namespace PropertiesPanel
{
	void onImGuiUpdate(entt::entity& selectedEntity, entt::registry& registry, AssetRegistry& assets, MRG::Renderer& renderer);
};  // namespace PropertiesPanel

#endif
