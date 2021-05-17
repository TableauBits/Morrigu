//
// Created by Mathis Lamidey on 2021-05-16.
//

#ifndef MORRIGU_FILENAMES_H
#define MORRIGU_FILENAMES_H

namespace MRG::Files
{
	namespace Rendering
	{
		static const std::string vkPipelineCacheFile = ".vkPipelineCache";
	}
}  // namespace MRG::Files

namespace MRG::Folders
{
	namespace Rendering
	{
		static const std::string assetsFolder = "assets/";
		static const std::string meshesFolder = assetsFolder + "meshes/";

		static const std::string shadersFolder = "shaders/";
	}  // namespace Rendering
}  // namespace MRG::Folders

#endif  // MORRIGU_FILENAMES_H
