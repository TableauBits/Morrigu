#include "SceneHierarchyPanel.h"

#include "Core/Warnings.h"
#include "Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace
{
	void drawVec3Control(const char* label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		auto& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize{lineHeight + 3.f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T>
	static void drawComponent(const char* name, MRG::Entity entity, void (*UIFunction)(T&))
	{
		const auto treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
		                           ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.hasComponent<T>()) {
			auto& component = entity.getComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
			auto lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name);
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open) {
				UIFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.removeComponent<T>();
		}
	}
}  // namespace

namespace MRG
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) { setContext(scene); }

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene)
	{
		m_context = scene;
		m_selectedEntity = {};
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_context->m_registry.each([&](const auto entityID) {
			Entity entity{entityID, m_context.get()};
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_selectedEntity = {};

		if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create empty entity"))
				m_context->createEntity("Empty entity");

			ImGui::EndPopup();
		}

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
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity, flags, "%s", tag.c_str());  // This is fucking ridiculous
		if (ImGui::IsItemClicked()) {
			m_selectedEntity = entity;
		}

		auto entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {
			flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			opened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted) {
			m_context->destroyEntity(entity);
			if (m_selectedEntity == entity)
				m_selectedEntity = {};
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
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				if (!m_selectedEntity.hasComponent<CameraComponent>())
					m_selectedEntity.addComponent<CameraComponent>();
				else
					MRG_ENGINE_WARN("Tried to add a CameraComponent to an entity that already has one!")
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Sprite renderer")) {
				if (!m_selectedEntity.hasComponent<SpriteRendererComponent>())
					m_selectedEntity.addComponent<SpriteRendererComponent>();
				else
					MRG_ENGINE_WARN("Tried to add a SpriteRendererComponent to an entity that already has one!")
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		drawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component) {
			drawVec3Control("Translation", component.translation);

			glm::vec3 rotation = glm::degrees(component.rotation);
			drawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);

			drawVec3Control("Scale", component.scale, 1.f);
		});

		drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component) {
			auto& camera = component.camera;

			ImGui::Checkbox("Primary", &component.primary);

			const char* projectionTypeStrings[] = {"Orthographic", "Perspective"};
			const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.getProjectionType())];
			if (ImGui::BeginCombo("Projection type", currentProjectionTypeString)) {
				for (auto i = 0; i < 2; ++i) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(i));
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				auto size = camera.getOrthographicSize();
				if (ImGui::DragFloat("Size", &size))
					camera.setOrthographicSize(size);
				auto OrthographicNear = camera.getOrthographicNear();
				if (ImGui::DragFloat("Near clip", &OrthographicNear))
					camera.setOrthographicNear(OrthographicNear);
				auto OrthographicFar = camera.getOrthographicFar();
				if (ImGui::DragFloat("Far clip", &OrthographicFar))
					camera.setOrthographicFar(OrthographicFar);
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
				auto verticalFOV = glm::degrees(camera.getPerspectiveFOV());
				if (ImGui::DragFloat("Vertical FOV", &verticalFOV, 1.f, 0.f, 180.f))
					camera.setPerspectiveFOV(glm::radians(verticalFOV));
				auto perspectiveNear = camera.getPerspectiveNear();
				if (ImGui::DragFloat("Near clip", &perspectiveNear))
					camera.setPerspectiveNear(perspectiveNear);
				auto perspectiveFar = camera.getPerspectiveFar();
				if (ImGui::DragFloat("Far clip", &perspectiveFar))
					camera.setPerspectiveFar(perspectiveFar);

				ImGui::Checkbox("Fixed aspect ratio", &component.fixedAspectRatio);
			}
		});

		drawComponent<SpriteRendererComponent>("Sprite renderer", entity, [](SpriteRendererComponent& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});
	}
}  // namespace MRG
