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
		CONAN_PKG::spdlog
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
