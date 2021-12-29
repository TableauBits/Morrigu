set(
		MRG_UTILS_SOURCES

		# Vulkan commands utilities
		${CMAKE_CURRENT_LIST_DIR}/Commands.h
		${CMAKE_CURRENT_LIST_DIR}/Commands.cpp

		# GLM include helper
		${CMAKE_CURRENT_LIST_DIR}/GLMIncludeHelper.h

		# Math helpers
		${CMAKE_CURRENT_LIST_DIR}/Maths.h
		${CMAKE_CURRENT_LIST_DIR}/Maths.cpp

		# Default meshes utilities
		${CMAKE_CURRENT_LIST_DIR}/Meshes.h
		${CMAKE_CURRENT_LIST_DIR}/Meshes.cpp

		# Utility layer implementations
		${CMAKE_CURRENT_LIST_DIR}/UtilityLayers.h
		${CMAKE_CURRENT_LIST_DIR}/UtilityLayers.cpp

		# VMA files to define implementation
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.cpp
)
