include(Morrigu/CMakeLists.txt)

add_library(
	Vendor
	${VENDOR_SOURCES}
)

target_include_directories(
	Vendor
	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings
)

target_link_libraries(
	Vendor
	PRIVATE
	Vulkan::Vulkan
	CONAN_PKG::glfw
	CONAN_PKG::imgui-docking
)

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
	Vendor
	Vulkan::Vulkan
	CONAN_PKG::freetype
	CONAN_PKG::glfw
	CONAN_PKG::glm
	CONAN_PKG::imgui-docking
	CONAN_PKG::spdlog
	CONAN_PKG::spirv-cross
	CONAN_PKG::stb
	CONAN_PKG::tinyobjloader
	CONAN_PKG::vk-bootstrap
	CONAN_PKG::vulkan-headers
	CONAN_PKG::vulkan-memory-allocator
)

set_property(TARGET Morrigu PROPERTY CXX_STANDARD 20)
set_property(TARGET Morrigu PROPERTY CXX_STANDARD_REQUIRED ON)
set_project_warnings(Morrigu)

include(Macha/CMakeLists.txt)

add_executable(
	Macha
	${MC_SOURCES}
)

target_link_libraries(
	Macha
	PRIVATE
	Morrigu
	CONAN_PKG::imguizmo
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Macha)
set_property(TARGET Macha PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime)

include(Shaders/CMakeLists.txt)

add_custom_command(
	TARGET Macha
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/Assets/Meshes ${CMAKE_SOURCE_DIR}/runtime/assets/meshes
)

add_custom_command(
	TARGET Macha
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/Assets/Textures ${CMAKE_SOURCE_DIR}/runtime/assets/textures
)

add_custom_command(
	TARGET Macha
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/Assets/Fonts ${CMAKE_SOURCE_DIR}/runtime/assets/fonts
)

add_dependencies(
	Macha
	Shaders
)

set_property(TARGET Macha PROPERTY CXX_STANDARD 20)
set_property(TARGET Macha PROPERTY CXX_STANDARD_REQUIRED ON)
set_project_warnings(Macha)
