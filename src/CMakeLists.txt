include("Morrigu/CMakeLists.txt")

add_library(
		Morrigu
		${MRG_SOURCES}
)

target_include_directories(
		Morrigu
		PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/Morrigu
)

target_link_libraries(
		Morrigu
		PUBLIC
		project_options
		project_warnings
		Vulkan::Vulkan
		CONAN_PKG::spdlog
		CONAN_PKG::glfw
		CONAN_PKG::vulkan-headers
		CONAN_PKG::vk-bootstrap
)

include("SampleApp/CMakeLists.txt")

add_executable(
		SampleApp
		${SA_SOURCES}
)

target_link_libraries(
		SampleApp
		PRIVATE
		project_options
		project_warnings
		Morrigu
)
