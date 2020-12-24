#include "SceneHierarchyPanel.h"

#include "Core/Warnings.h"
#include "Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace
{
	static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize{lineHeight + 3.f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}  // namespace

namespace MRG
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) { setContext(scene); }

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene) { m_context = scene; }

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_context->m_registry.each([&](const auto entityID) {
			Entity entity{entityID, m_context.get()};
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_selectedEntity = {};

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_selectedEntity)
			drawComponents(m_selectedEntity);

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity& entity)
	{
		const auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity, flags, tag.c_str());  // This is fucking ridiculous
		if (ImGui::IsItemClicked()) {
			m_selectedEntity = entity;
		}

		if (opened) {
			flags = ImGuiTreeNodeFlags_OpenOnArrow;
			opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity& entity)
	{
		if (entity.hasComponent<TagComponent>()) {
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			DISABLE_WARNING_PUSH
			DISABLE_WARNING_UNSAFE_FUNCTIONS
			strncat(buffer, tag.c_str(), tag.length());
			DISABLE_WARNING_POP
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				auto& tc = entity.getComponent<TransformComponent>();

				drawVec3Control("Translation", tc.translation);

				glm::vec3 rotation = glm::degrees(tc.rotation);
				drawVec3Control("Rotation", rotation);
				tc.rotation = glm::radians(rotation);

				drawVec3Control("Scale", tc.scale, 1.f);

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {
				auto& cc = entity.getComponent<CameraComponent>();
				auto& camera = cc.camera;

				ImGui::Checkbox("Primary", &cc.primary);

				const char* projectionTypeStrings[] = {"Orthographic", "Perspective"};
				const char* currentProjectionString = projectionTypeStrings[static_cast<int>(camera.getProjectionType())];
				if (ImGui::BeginCombo("Projection", currentProjectionString)) {
					for (int i = 0; i < 2; ++i) {
						auto isSelected = currentProjectionString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionString = projectionTypeStrings[i];
							camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(i));
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					auto size = glm::degrees(camera.getOrthographicSize());
					if (ImGui::DragFloat("Size", &size))
						camera.setOrthographicSize(glm::radians(size));
					auto OrthographicNear = glm::degrees(camera.getOrthographicNear());
					if (ImGui::DragFloat("Near clip", &OrthographicNear))
						camera.setOrthographicNear(glm::radians(OrthographicNear));
					auto OrthographicFar = glm::degrees(camera.getOrthographicFar());
					if (ImGui::DragFloat("Far clip", &OrthographicFar))
						camera.setOrthographicFar(glm::radians(OrthographicFar));
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
					auto verticalFOV = glm::degrees(camera.getPerspectiveFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFOV))
						camera.setPerspectiveFOV(glm::radians(verticalFOV));
					auto perspectiveNear = glm::degrees(camera.getPerspectiveNear());
					if (ImGui::DragFloat("Near clip", &perspectiveNear))
						camera.setPerspectiveNear(glm::radians(perspectiveNear));
					auto perspectiveFar = glm::degrees(camera.getPerspectiveFar());
					if (ImGui::DragFloat("Far clip", &perspectiveFar))
						camera.setPerspectiveFar(glm::radians(perspectiveFar));

					ImGui::Checkbox("Fixed aspect ratio", &cc.fixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer")) {
				auto& src = entity.getComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(src.color));
				ImGui::TreePop();
			}
		}
	}
}  // namespace MRG
