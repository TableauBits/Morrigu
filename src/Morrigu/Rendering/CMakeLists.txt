set(
		MRG_RENDERING_SOURCES

		# Vulkan types header
		${CMAKE_CURRENT_LIST_DIR}/VkTypes.h

		# Vulkan types initialization helper
		${CMAKE_CURRENT_LIST_DIR}/VkInitialize.h
		${CMAKE_CURRENT_LIST_DIR}/VkInitialize.cpp

		# Pipeline builder class
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.h
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.cpp

		# Vulkan renderer files
		${CMAKE_CURRENT_LIST_DIR}/VkRenderer.h
		${CMAKE_CURRENT_LIST_DIR}/VkRenderer.cpp
)
