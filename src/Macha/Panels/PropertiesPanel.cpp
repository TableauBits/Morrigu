//
// Created by Mathis Lamidey on 2021-11-23
//

#include "PropertiesPanel.h"

#include "Components/EntitySettings.h"
#include "Components/LightCasters/DirectionalLight.h"
#include "Components/LightCasters/PointLight.h"

#include "Vendor/ImGui/misc/imgui_stdlib.h"
#include <imgui_internal.h>

#include <filesystem>

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

		[[nodiscard]] bool removeComponent(const char* componentName)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, {0.55f, 0.1f, 0.1f, 1.f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.65f, 0.1f, 0.1f, 1.f});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.8f, 0.1f, 0.1f, 1.f});

			static constexpr auto padding = 10.f;
			ImGui::SetCursorPosX(padding / 2.f);
			const auto res =
			  ImGui::Button(fmt::format("Remove component##{}", componentName).c_str(), {ImGui::GetContentRegionAvail().x - padding, 0.f});

			ImGui::PopStyleColor(3);

			return res;
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

	[[nodiscard]] bool editMeshRendererComponent(MRG::Components::MeshRenderer<MRG::TexturedVertex>& mrc,
	                                             MRG::Components::Transform& tc,
	                                             Components::EntitySettings& esc,
	                                             AssetRegistry& assets,
	                                             MRG::Renderer& renderer)
	{
		ImGui::Dummy({0.f, 20.f});
		if (ImGui::CollapsingHeader("Mesh renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGuiUtils::removeComponent("mesh renderer")) { return true; }

			ImGui::PushID("Mesh DnD Target");
			ImGui::Text("Drop new mesh here");  // @TODO(Ithyx): Display mesh name (path ?)
			if (ImGui::BeginDragDropTarget()) {
				if (const auto* payload = ImGui::AcceptDragDropPayload("ASSET_PANEL")) {
					const auto meshPath =
					  std::filesystem::relative(reinterpret_cast<const char*>(payload->Data), MRG::Folders::Rendering::meshesFolder)
					    .string();
					MRG_ENGINE_TRACE("Drag and drop payload to mesh received: {}", meshPath)

					auto mesh = MRG::Utils::Meshes::loadMeshFromFile<MRG::TexturedVertex>(meshPath.c_str());
					renderer.uploadMesh(mesh);

					mrc.mesh = mesh;
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();

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
				const auto name = mrc.material->shader->l3ImageBindings[textureBindingInfo.first].name.c_str();
				ImGui::PushID(name);
				ImGui::Text("%s: %s", name, textureBindingInfo.second->path.c_str());
				if (ImGui::BeginDragDropTarget()) {
					if (const auto* payload = ImGui::AcceptDragDropPayload("ASSET_PANEL")) {
						const auto texturePath =
						  std::filesystem::relative(reinterpret_cast<const char*>(payload->Data), MRG::Folders::Rendering::texturesFolder)
						    .string();
						MRG_ENGINE_TRACE("Drag and drop payload to texture received: {}", texturePath)

						auto texture = assets.getTexture(texturePath);
						MRG::Ref<MRG::Texture> textureValue;
						if (texture.has_value()) {
							textureValue = texture.value();
						} else {
							textureValue = renderer.createTexture(texturePath.c_str());
							assets.addTexture(textureValue);
						}

						mrc.bindTexture(textureBindingInfo.first, textureValue);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
			}
		}
		mrc.updateTransform(tc.getTransform());
		return false;
	}

	[[nodiscard]] bool editDirectionalLightComponent(Components::DirectionalLight& dlc)
	{
		ImGui::Dummy({0.f, 20.f});
		if (ImGui::CollapsingHeader("Directional light", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGuiUtils::removeComponent("directional light")) { return true; }

			ImGui::ColorEdit3("light color", glm::value_ptr(dlc.color));
		}
		return false;
	}
}  // namespace

namespace PropertiesPanel
{
	void onImGuiUpdate(entt::entity& selectedEntity, entt::registry& registry, AssetRegistry& assets, MRG::Renderer& renderer)
	{
		if (ImGui::Begin("Entity properties")) {
			if (selectedEntity == entt::null) {
				ImGui::End();
				return;
			}

			if (ImGui::BeginPopupContextWindow("Component add popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				if (ImGui::BeginMenu("Add component")) {
					if (!registry.all_of<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(selectedEntity) &&
					    ImGui::MenuItem("Mesh renderer")) {
						auto mesh = MRG::Utils::Meshes::cube<MRG::TexturedVertex>();
						renderer.uploadMesh(mesh);
						registry.emplace<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(
						  selectedEntity, renderer.createMeshRenderer(mesh, renderer.defaultTexturedMaterial));
					}

					if (!registry.all_of<Components::DirectionalLight>(selectedEntity) && ImGui::MenuItem("Directional Light")) {
						registry.emplace<Components::DirectionalLight>(selectedEntity, glm::vec4{1.f});
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
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
				if (editMeshRendererComponent(mrc, tc, esc, assets, renderer)) {
					registry.remove<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(selectedEntity);
				}
			}
			if (registry.all_of<Components::DirectionalLight>(selectedEntity)) {
				auto& dlc = registry.get<Components::DirectionalLight>(selectedEntity);
				if (editDirectionalLightComponent(dlc)) { registry.remove<Components::DirectionalLight>(selectedEntity); }
			}
		}
		ImGui::End();
	}
}  // namespace PropertiesPanel
