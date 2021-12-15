//
// Created by Mathis Lamidey on 2021-11-22
//

#include "HierarchyPanel.h"

void HierarchyPanel::onImGuiUpdate(const std::vector<MRG::Entity>& entities)
{
	if (ImGui::Begin("Scene hierarchy")) {
		if (ImGui::BeginPopupContextWindow("Entity creation popup",
		                                   ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
			if (ImGui::MenuItem("Create new entity")) { callbacks.entityCreation(); }

			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0.f, 5.f});
		for (const auto& entity : entities) { renderEntity(entity); }
		if (ImGui::IsMouseDown(MRG::Mouse::ButtonLeft) && ImGui::IsWindowHovered()) { modifySelectedEntity(entt::null); }
		ImGui::PopStyleVar(2);
	}

	ImGui::End();
}

void HierarchyPanel::renderEntity(const MRG::Entity& entity)
{
	int flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if (entity == selectedEntity) { flags |= ImGuiTreeNodeFlags_Selected; }

	// I pray that God forgives ImGui, because he sure knows I can't
	const auto opened =
	  ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entity.getID())), flags, "%s", entity.getName().c_str());
	if (ImGui::IsItemClicked()) { modifySelectedEntity(entity.getHandle()); }
	if (opened) { ImGui::TreePop(); }
}

void HierarchyPanel::modifySelectedEntity(const entt::entity& entity)
{
	selectedEntity = entity;
	callbacks.entitySelected(entity);
}
