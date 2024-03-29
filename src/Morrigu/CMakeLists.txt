include(${CMAKE_CURRENT_LIST_DIR}/Core/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Entity/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Events/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Rendering/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Utils/CMakeLists.txt)

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
