set(
		MRG_UTILS_SOURCES

		# Allocation utilities
		${CMAKE_CURRENT_LIST_DIR}/Allocators.h
		${CMAKE_CURRENT_LIST_DIR}/Allocators.cpp

		# GLM include helper
		${CMAKE_CURRENT_LIST_DIR}/GLMIncludeHelper.h

		# Default meshes utilities
		${CMAKE_CURRENT_LIST_DIR}/Meshes.h
		${CMAKE_CURRENT_LIST_DIR}/Meshes.cpp

		# Utility layer implementations
		${CMAKE_CURRENT_LIST_DIR}/UtilityLayers.h
		${CMAKE_CURRENT_LIST_DIR}/UtilityLayers.cpp

		# STB files to define implementation
		${CMAKE_CURRENT_LIST_DIR}/STBIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/STBIncludeHelper.cpp

		# VMA files to define implementation
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.cpp
)
