set(
		MRG_UTILS_SOURCES

		# Allocation utilities
		${CMAKE_CURRENT_LIST_DIR}/Allocators.h
		${CMAKE_CURRENT_LIST_DIR}/Allocators.cpp

		# GLM include helper
		${CMAKE_CURRENT_LIST_DIR}/GLMIncludeHelper.h

		# Model loading utilities
		${CMAKE_CURRENT_LIST_DIR}/ModelLoading.h
		${CMAKE_CURRENT_LIST_DIR}/ModelLoading.cpp

		# STB files to define implementation
		${CMAKE_CURRENT_LIST_DIR}/STBIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/STBIncludeHelper.cpp

		# VMA files to define implementation
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.cpp
)
