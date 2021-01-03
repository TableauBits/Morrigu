#ifndef MRG_UTILS_FILEDIALOGS
#define MRG_UTILS_FILEDIALOGS

#include <optional>
#include <string>
#include <vector>

namespace MRG
{
	class FileDialogs
	{
	public:
		[[nodiscard]] static std::string openFile(const char* actionTitle,
		                                          const char* filterDescription,
		                                          const std::vector<const char*>& filters,
		                                          const char* defaultPath = "");
		[[nodiscard]] static std::string saveFile(const char* actionTitle,
		                                          const char* filterDescription,
		                                          const std::vector<const char*>& filters,
		                                          const char* defaultFile = "");
	};
}  // namespace MRG

#endif
