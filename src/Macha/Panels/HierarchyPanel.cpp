//
// Created by Mathis Lamidey on 2021-11-22
//

#include "HierarchyPanel.h"

void HierarchyPanel::onImGuiUpdate(entt::registry& registry, const entt::entity selectedEntity)
{
	if (ImGui::Begin("Scene hierarchy")) {
		if (ImGui::BeginPopupContextWindow("Entity creation popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create new entity")) { callbacks.entityCreation(); }

			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 0.f});
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0.f, 5.f});
		registry.each(
		  [this, &registry, selectedEntity](const entt::entity entity) { renderEntity(registry, entity, entity == selectedEntity); });
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

void HierarchyPanel::renderEntity(entt::registry& registry, const entt::entity entity, bool isSelected)
{
	int flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	if (isSelected) { flags |= ImGuiTreeNodeFlags_Selected; }

	// I pray that God forgives ImGui, because he sure knows I can't
	const auto& tc    = registry.get<MRG::Components::Tag>(entity);
	const auto opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(entity)), flags, "%s", tc.tag.c_str());
	if (ImGui::IsItemClicked()) { modifySelectedEntity(entity); }
	if (ImGui::BeginPopupContextItem("Entity options popup", ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Delete entity")) { m_entityToDelete = entity; }

		ImGui::EndPopup();
	}
	if (opened) { ImGui::TreePop(); }
}

void HierarchyPanel::modifySelectedEntity(const entt::entity entity) { callbacks.entitySelected(entity); }
