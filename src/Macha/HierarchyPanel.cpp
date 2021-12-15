//
// Created by Mathis Lamidey on 2021-11-22
//

#include "HierarchyPanel.h"

void HierarchyPanel::onImGuiUpdate(const std::unordered_map<entt::entity, MRG::Entity>& entities)
{
	if (ImGui::Begin("Scene hierarchy")) {
		if (ImGui::BeginPopupContextWindow("Entity creation popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create new entity")) { callbacks.entityCreation(); }

			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0.f, 5.f});
		for (const auto& [_, entity] : entities) { renderEntity(entity); }
		if (ImGui::IsMouseDown(MRG::Mouse::ButtonLeft) && ImGui::IsWindowHovered()) { modifySelectedEntity(entt::null); }
		ImGui::PopStyleVar(2);
	}

	if (m_entityToDelete) {
		const auto handle = m_entityToDelete.value();
		if (selectedEntity == handle) { modifySelectedEntity(entt::null); }
		callbacks.entityDestruction(handle);

		m_entityToDelete.reset();
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
	if (ImGui::BeginPopupContextItem("Entity options popup", ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Delete entity")) { m_entityToDelete = entity.getHandle(); }

		ImGui::EndPopup();
	}
	if (opened) { ImGui::TreePop(); }
}

void HierarchyPanel::modifySelectedEntity(const entt::entity& entity)
{
	selectedEntity = entity;
	callbacks.entitySelected(entity);
}
