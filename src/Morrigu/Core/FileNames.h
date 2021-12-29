//
// Created by Mathis Lamidey on 2021-05-16.
//

#ifndef MORRIGU_FILENAMES_H
#define MORRIGU_FILENAMES_H

#include <filesystem>
#include <string>

namespace MRG::Files
{
	namespace Rendering
	{
		static const std::string vkPipelineCacheFile = ".vkPipelineCache";
		static const std::string defaultTexture      = std::filesystem::path{"engine/default.png"}.make_preferred().string();
	}  // namespace Rendering
}  // namespace MRG::Files

namespace MRG::Folders
{
	namespace Rendering
	{
		static const std::string assetsFolder   = std::filesystem::path{"assets/"}.make_preferred().string();
		static const std::string meshesFolder   = assetsFolder + std::filesystem::path{"meshes/"}.make_preferred().string();
		static const std::string texturesFolder = assetsFolder + std::filesystem::path{"textures/"}.make_preferred().string();

		static const std::string shadersFolder = std::filesystem::path{"shaders/"}.make_preferred().string();
	}  // namespace Rendering
}  // namespace MRG::Folders

#endif  // MORRIGU_FILENAMES_H
