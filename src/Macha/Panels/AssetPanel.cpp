#include "AssetPanel.h"

#include <imgui_internal.h>

AssetPanel::AssetPanel(MRG::Renderer& renderer)
{
	m_iconFile      = renderer.createTexture("editor/icons/asset-file.png");
	m_iconFolder    = renderer.createTexture("editor/icons/asset-folder.png");
	m_iconParentDir = renderer.createTexture("editor/icons/asset-parent-dir.png");

	m_currentPath = MRG::Folders::Rendering::assetsFolder;
}

void AssetPanel::onImGuiRender()
{
	if (ImGui::Begin("Asset browser")) {
		static const auto padding     = 16.f;
		static const auto upSize      = 16.f;
		static const auto elementSize = 100.f;
		const auto totalElementSize   = elementSize + padding;
		const auto baseFolder         = std::filesystem::path{MRG::Folders::Rendering::assetsFolder};

		ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
		ImGui::BeginDisabled(std::filesystem::equivalent(m_currentPath, baseFolder));
		if (ImGui::ImageButton(m_iconParentDir->getImTexID(), {upSize, upSize})) { m_currentPath = m_currentPath.parent_path(); }
		ImGui::EndDisabled();
		ImGui::PopStyleColor();

		const auto totalWidth  = ImGui::GetContentRegionAvail().x;
		const auto columnCount = std::max(1, static_cast<int>(totalWidth / totalElementSize));

		auto counter = 0;
		if (ImGui::BeginTable("Current folder assets", columnCount)) {
			for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
				const auto path = entry.path().string();

				ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
				ImGui::PushID(path.c_str());

				if (counter % columnCount == 0) { ImGui::TableNextRow(); }
				ImGui::TableNextColumn();

				auto icon = (entry.is_regular_file()) ? m_iconFile : m_iconFolder;
				ImGui::ImageButton(icon->getImTexID(), {elementSize, elementSize});

				if (entry.is_regular_file()) {
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("ASSET_PANEL", path.c_str(), path.size() + 1, ImGuiCond_Once);
						ImGui::Text("%s", path.c_str());
						ImGui::EndDragDropSource();
					}
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(MRG::Mouse::ButtonLeft) && entry.is_directory()) {
					m_currentPath = entry.path();
				}

				ImGui::TextWrapped("%s", path.c_str());

				ImGui::PopID();
				ImGui::PopStyleColor();

				++counter;
			}
		}
		ImGui::EndTable();
	}

	ImGui::End();
}
