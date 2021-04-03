add_executable(
		Morrigu
		main.cpp
)

target_link_libraries(
		Morrigu
		PRIVATE project_options
				project_warnings
				CONAN_PKG::glfw
)
