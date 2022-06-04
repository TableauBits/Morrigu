include(${CMAKE_CURRENT_LIST_DIR}/Core/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Entity/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Events/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Rendering/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Utils/CMakeLists.txt)

# Patching ImGui:
# @TODO(Ithyx): Move this horror to a conan.py
file(READ ${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_vulkan.cpp VULKAN_BACKEND)
message("Patching ImGui")
string(
	REPLACE
	"//ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline, bd->Subpass)"
	"ImGui_ImplVulkan_Data* bd = ImGui_ImplVulkan_GetBackendData();\n        ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline, bd->Subpass)"
	VULKAN_BACKEND
	"${VULKAN_BACKEND}"
)
string(
	REPLACE
	"info.format = VK_FORMAT_R8G8B8A8_UNORM;"
	"info.format = VK_FORMAT_R8G8B8A8_SRGB;"
	VULKAN_BACKEND
	"${VULKAN_BACKEND}"
)
string(
	REPLACE
	"{ VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM }"
	"{ VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8_SRGB, VK_FORMAT_R8G8B8_SRGB }"
	VULKAN_BACKEND
	"${VULKAN_BACKEND}"
)
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_vulkan.cpp "${VULKAN_BACKEND}")

set(
	IMGUI_BINDINGS
 
	# ImGui bindings
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_glfw.h
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_vulkan.h
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/bindings/imgui_impl_vulkan.cpp
)

set(
	IMGUI_MISC

	# ImGui misc sources
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/misc/imgui_stdlib.h
	${CMAKE_CURRENT_LIST_DIR}/Vendor/ImGui/misc/imgui_stdlib.cpp
)

set(
	MRG_SOURCES

	# Global include file
	${CMAKE_CURRENT_LIST_DIR}/Morrigu.h

	# Core files
	${MRG_CORE_SOURCES}

	# Entity files
	${MRG_ENTITY_SOURCES}

	# Event files
	${MRG_EVENT_SOURCES}

	# Rendering files
	${MRG_RENDERING_SOURCES}

	# Utils files
	${MRG_UTILS_SOURCES}
)

set(
	VENDOR_SOURCES

	# ImGui bindings
	${IMGUI_BINDINGS}

	# ImGui misc
	${IMGUI_MISC}
)
