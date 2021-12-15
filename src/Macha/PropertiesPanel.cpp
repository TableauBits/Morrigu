//
// Created by Mathis Lamidey on 2021-11-23
//

#include "PropertiesPanel.h"

#include "Components/EntitySettings.h"

#include "Vendor/ImGui/misc/imgui_stdlib.h"
#include <imgui_internal.h>

namespace
{
	namespace ImGuiUtils
	{
		// From: https://stackoverflow.com/a/67855985
		void centeredText(const std::string& text)
		{
			auto winWidth = ImGui::GetWindowWidth();
			auto textSize = ImGui::CalcTextSize(text.c_str()).x;

			ImGui::SetCursorPosX((winWidth - textSize) * 0.5f);
			ImGui::Text("%s", text.c_str());
		}

		void subsectionHeader(const std::string& title)
		{
			ImGui::Dummy({0.f, 5.f});
			centeredText(title);
		}
	}  // namespace ImGuiUtils

	void
	drawVec3Controls(const std::string& id, const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		ImGuiIO& io   = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(id.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label.c_str());
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

	void renderUBOData(const MRG::Shader::Node& node, std::byte*& rwHead)
	{
		if (node.members.empty()) {
			switch (node.type.basetype) {
			case spirv_cross::SPIRType::BaseType::Float: {
				auto* pointerCast = reinterpret_cast<float*>(rwHead);
				switch (node.type.vecsize) {
				case 1: {
					// Single float
					ImGui::DragFloat(node.name.c_str(), pointerCast, 0.01f, 0.f, 1.f);
					rwHead += 1 * sizeof(float);
				} break;
				case 2: {
					// vec2
					ImGui::DragFloat2(node.name.c_str(), pointerCast);
					rwHead += 2 * sizeof(float);
				} break;
				case 4: {
					// vec4
					ImGui::ColorEdit4(node.name.c_str(), pointerCast);
					rwHead += 4 * sizeof(float);
				} break;
				default: {
					MRG_WARN("Only scalar float and vec2/4 are supported!")
				} break;
				}
			} break;

			default: {
				MRG_WARN("Shader data type not supported yet!")
			} break;
			}
		} else {
			if (ImGui::TreeNode(node.name.c_str())) {
				for (const auto& member : node.members) { renderUBOData(member, rwHead); }
				ImGui::TreePop();
			}
		}
	}

	void editMeshRendererComponent(MRG::Components::MeshRenderer<MRG::TexturedVertex>& mrc,
	                               MRG::Components::Transform& tc,
	                               Components::EntitySettings& esc)
	{
		ImGui::Dummy({0.f, 20.f});
		if (ImGui::CollapsingHeader("Mesh renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Visible", &mrc.isVisible);

			ImGuiUtils::subsectionHeader("Offset transform");
			drawVec3Controls("MRC.t", "Translation", mrc.offset.translation);
			drawVec3Controls("MRC.r", "Rotation", mrc.offset.rotation);
			drawVec3Controls("MRC.s", "Scale", mrc.offset.scale, 1.f);

			ImGuiUtils::subsectionHeader("L3 material parameters (Per entity)");
			ImGuiUtils::centeredText("Uniform data");
			if (esc.uboData.size() != mrc.material->shader->l3UBOData.size()) {
				esc.uboData.resize(mrc.material->shader->l3UBOData.size());
				std::size_t index = 0;
				for (auto& ubo : mrc.material->shader->l3UBOData) {
					esc.uboData[index].resize(ubo.second.size);
					++index;
				}
			}
			std::size_t index = 0;
			for (const auto& ubo : mrc.material->shader->l3UBOData) {
				auto* rwHead = esc.uboData[index].data();
				if (ImGui::TreeNode(ubo.second.name.c_str())) {
					for (const auto& member : ubo.second.members) { renderUBOData(member, rwHead); }
					ImGui::TreePop();
				}

				mrc.uploadUniform(ubo.first, esc.uboData[index].data(), esc.uboData[index].size());
				++index;
			}

			ImGuiUtils::centeredText("Textures bindings");
			for (const auto& textureBindingInfo : mrc.sampledImages) {
				ImGui::Text("%s: %s",
				            mrc.material->shader->l3ImageBindings[textureBindingInfo.first].name.c_str(),
				            textureBindingInfo.second->path.c_str());
			}
		}
		mrc.updateTransform(tc.getTransform());
	}
}  // namespace

namespace PropertiesPanel
{
	void onImGuiUpdate(entt::entity& selectedEntity, entt::registry& registry)
	{
		if (ImGui::Begin("Entity properties")) {
			if (selectedEntity == entt::null) {
				ImGui::End();
				return;
			}

			// Tag and Transform components are guaranteed to be present on an entity
			ImGui::InputText("Name", &registry.get<MRG::Components::Tag>(selectedEntity).tag);

			auto& tc  = registry.get<MRG::Components::Transform>(selectedEntity);
			auto& esc = registry.get<Components::EntitySettings>(selectedEntity);
			ImGuiUtils::centeredText("Transform");
			drawVec3Controls("TC.t", "Translation", tc.translation);
			drawVec3Controls("TC.r", "Rotation", tc.rotation);
			drawVec3Controls("TC.s", "Scale", tc.scale, 1.f);

			if (registry.all_of<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(selectedEntity)) {
				auto& mrc = registry.get<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(selectedEntity);
				editMeshRendererComponent(mrc, tc, esc);
			}
		}
		ImGui::End();
	}
}  // namespace PropertiesPanel
