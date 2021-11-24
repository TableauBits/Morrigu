//
// Created by Mathis Lamidey on 2021-11-22
//

#include "HierarchyPanel.h"

void HierarchyPanel::onImGuiUpdate(const std::vector<MRG::Ref<MRG::Entity>>& entities)
{
	if (ImGui::Begin("Scene hierarchy")) {}

	for (const auto& entity : entities) { renderEntity(entity); }
	if (ImGui::IsMouseDown(MRG::Mouse::ButtonLeft) && ImGui::IsWindowHovered()) { modifySelectedEntity(nullptr); }

	ImGui::End();
}

void HierarchyPanel::renderEntity(const MRG::Ref<MRG::Entity>& entity)
{
	const auto name = entity->getName();

	int flags = ImGuiTreeNodeFlags_Leaf;  // ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (entity == selectedEntity) { flags |= ImGuiTreeNodeFlags_Selected; }

	// I pray that God forgives ImGui, because he sure knows I can't
	const auto opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entity->getID())), flags, "%s", name.c_str());
	if (ImGui::IsItemClicked()) { modifySelectedEntity(entity); }
	if (opened) { ImGui::TreePop(); }
}

void HierarchyPanel::modifySelectedEntity(const MRG::Ref<MRG::Entity>& entity)
{
	// @TODO(Ithyx): callback
	selectedEntity = entity;
}
