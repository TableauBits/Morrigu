set(
		MRG_RENDERING_SOURCES

		# Camera class
		${CMAKE_CURRENT_LIST_DIR}/Camera.h
		${CMAKE_CURRENT_LIST_DIR}/Camera.cpp

		# Framebuffer class
		${CMAKE_CURRENT_LIST_DIR}/Framebuffer.h
		${CMAKE_CURRENT_LIST_DIR}/Framebuffer.cpp

		# Material class
		${CMAKE_CURRENT_LIST_DIR}/Material.h

		# Mesh class
		${CMAKE_CURRENT_LIST_DIR}/Mesh.h

		# Pipeline builder class
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.h
		${CMAKE_CURRENT_LIST_DIR}/PipelineBuilder.cpp

		# Shader class
		${CMAKE_CURRENT_LIST_DIR}/Shader.h
		${CMAKE_CURRENT_LIST_DIR}/Shader.cpp

		# Texture class
		${CMAKE_CURRENT_LIST_DIR}/Texture.h
		${CMAKE_CURRENT_LIST_DIR}/Texture.cpp

		# Vertex file
		${CMAKE_CURRENT_LIST_DIR}/Vertex.h
		${CMAKE_CURRENT_LIST_DIR}/Vertex.cpp

		# Renderer class
		${CMAKE_CURRENT_LIST_DIR}/Renderer.h
		${CMAKE_CURRENT_LIST_DIR}/Renderer.cpp

		# Rendering types
		${CMAKE_CURRENT_LIST_DIR}/RendererTypes.h
		${CMAKE_CURRENT_LIST_DIR}/RendererTypes.cpp
)
