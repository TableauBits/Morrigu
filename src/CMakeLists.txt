include("Morrigu/CMakeLists.txt")

add_library(
		Morrigu
		${MRG_SOURCES}
)

target_link_libraries(
		Morrigu
		PRIVATE
		project_options
		project_warnings
		CONAN_PKG::glfw
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
