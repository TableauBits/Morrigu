set(
		MRG_RENDERING_SOURCES

		# Vulkan types header
		${CMAKE_CURRENT_LIST_DIR}/VkTypes.h

		# GLM include helper
		${CMAKE_CURRENT_LIST_DIR}/GLMIncludeHelper.h

		# VMA file to define implementation
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.h
		${CMAKE_CURRENT_LIST_DIR}/VMAIncludeHelper.cpp

		# Vulkan types initialization helper
		${CMAKE_CURRENT_LIST_DIR}/VkInitialize.h
		${CMAKE_CURRENT_LIST_DIR}/VkInitialize.cpp

		# Pipeline builder class
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.h
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.cpp

		# Mesh class
		${CMAKE_CURRENT_LIST_DIR}/Mesh.h
		${CMAKE_CURRENT_LIST_DIR}/Mesh.cpp

		# Camera class
		${CMAKE_CURRENT_LIST_DIR}/Camera.h
		${CMAKE_CURRENT_LIST_DIR}/Camera.cpp

		# Vulkan renderer files
		${CMAKE_CURRENT_LIST_DIR}/VkRenderer.h
		${CMAKE_CURRENT_LIST_DIR}/VkRenderer.cpp
)
