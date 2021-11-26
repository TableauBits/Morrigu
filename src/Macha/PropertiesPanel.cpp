//
// Created by Mathis Lamidey on 2021-11-23
//

#include "PropertiesPanel.h"

#include "Components/EntitySettings.h"

#include "Vendor/ImGui/misc/imgui_stdlib.h"
#include <imgui_internal.h>

namespace
{
	// From: https://stackoverflow.com/a/67855985
	void centeredText(const std::string& text)
	{
		auto winWidth = ImGui::GetWindowWidth();
		auto textSize = ImGui::CalcTextSize(text.c_str()).x;

		ImGui::SetCursorPosX((winWidth - textSize) * 0.5f);
		ImGui::Text(text.c_str());
	}

	void
	drawVec3Controls(const std::string& id, const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		ImGuiIO& io   = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(id.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight  = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void editMeshRendererComponent(MRG::Components::MeshRenderer<MRG::TexturedVertex>& mrc, MRG::Components::Transform& tc)
	{
		ImGui::Dummy({0.f, 20.f});
		if (ImGui::CollapsingHeader("Mesh renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Visible", &mrc.isVisible);

			centeredText("Offset");
			drawVec3Controls("MRC.t", "Translation", mrc.offset.translation);
			drawVec3Controls("MRC.r", "Rotation", mrc.offset.rotation);
			drawVec3Controls("MRC.s", "Scale", mrc.offset.scale, 1.f);
		}
		mrc.updateTransform(tc.getTransform());
	}
}  // namespace

namespace PropertiesPanel
{
	void onImGuiUpdate(MRG::Ref<MRG::Entity>& selectedEntity)
	{
		if (ImGui::Begin("Entity properties")) {
			if (selectedEntity == nullptr) {
				ImGui::End();
				return;
			}

			// Tag and Transform components are guaranteed to be present on an entity
			ImGui::InputText("Name", &selectedEntity->getComponent<MRG::Components::Tag>().tag);

			auto& tc = selectedEntity->getComponent<MRG::Components::Transform>();
			centeredText("Transform");
			drawVec3Controls("TC.t", "Translation", tc.translation);
			drawVec3Controls("TC.r", "Rotation", tc.rotation);
			drawVec3Controls("TC.s", "Scale", tc.scale, 1.f);

			if (selectedEntity->hasComponents<MRG::Components::MeshRenderer<MRG::TexturedVertex>>()) {
				auto& mrc = selectedEntity->getComponent<MRG::Components::MeshRenderer<MRG::TexturedVertex>>();
				editMeshRendererComponent(mrc, tc);
			}
		}
		ImGui::End();
	}
}  // namespace PropertiesPanel
