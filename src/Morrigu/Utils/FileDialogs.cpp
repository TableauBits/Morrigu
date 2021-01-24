#include "FileDialogs.h"

// I might plan on implementing these function with native code, but I prefer using this for now.
#include <TinyFileDialogs/tinyfiledialogs.h>

namespace MRG
{
	std::optional<std::string> FileDialogs::openFile(const char* actionTitle,
	                                                 const char* filterDescription,
	                                                 const std::vector<const char*>& filters,
	                                                 const char* defaultPath)
	{
		const auto result =
		  tinyfd_openFileDialog(actionTitle, defaultPath, static_cast<int>(filters.size()), filters.data(), filterDescription, 0);
		return (result != nullptr) ? std::optional<std::string>{result} : std::nullopt;
	}

	std::optional<std::string> FileDialogs::saveFile(const char* actionTitle,
	                                                 const char* filterDescription,
	                                                 const std::vector<const char*>& filters,
	                                                 const char* defaultFile)
	{
		const auto result =
		  tinyfd_saveFileDialog(actionTitle, defaultFile, static_cast<int>(filters.size()), filters.data(), filterDescription);
		return (result != nullptr) ? std::optional<std::string>{result} : std::nullopt;
	}
}  // namespace MRG
