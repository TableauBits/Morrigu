#include "SceneHierarchyPanel.h"

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
}  // namespace MRG
