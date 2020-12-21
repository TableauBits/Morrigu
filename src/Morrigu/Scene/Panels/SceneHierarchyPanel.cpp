#include "SceneHierarchyPanel.h"

#include "Core/Warnings.h"
#include "Scene/Components.h"

#include <imgui.h>

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
				auto& transform = entity.getComponent<TransformComponent>().transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

				ImGui::TreePop();
			}
		}
	}
}  // namespace MRG
