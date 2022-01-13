include(${CMAKE_CURRENT_LIST_DIR}/Components/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Panels/CMakeLists.txt)

set(
		MC_SOURCES

		# Components
		${MC_COMPONENT_SOURCES}

		# Panels
		${MC_PANEL_SOURCES}

		# Asset Registry
		${CMAKE_CURRENT_LIST_DIR}/AssetRegistry.h
		${CMAKE_CURRENT_LIST_DIR}/AssetRegistry.cpp
		
		# Main file
		${CMAKE_CURRENT_LIST_DIR}/Macha.cpp

		# Scene
		${CMAKE_CURRENT_LIST_DIR}/Scene.h
		${CMAKE_CURRENT_LIST_DIR}/Scene.cpp
)
