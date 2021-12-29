//
// Created by Mathis Lamidey on 2021-12-15
//

#ifndef ASSET_PANEL_H
#define ASSET_PANEL_H

#include <Morrigu.h>

#include <filesystem>

class AssetPanel
{
public:
	AssetPanel(MRG::Renderer& renderer);

	void onImGuiRender();

private:
	MRG::Ref<MRG::Texture> m_iconFile, m_iconFolder, m_iconParentDir;
	std::filesystem::path m_currentPath{};
};

#endif
