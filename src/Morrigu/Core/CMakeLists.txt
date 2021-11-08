set(
		MRG_CORE_SOURCES

		# Main definitions
		${CMAKE_CURRENT_LIST_DIR}/PlatformDetection.h
		${CMAKE_CURRENT_LIST_DIR}/Warnings.h
		${CMAKE_CURRENT_LIST_DIR}/Core.h
		${CMAKE_CURRENT_LIST_DIR}/MouseCodes.h
		${CMAKE_CURRENT_LIST_DIR}/KeyCodes.h
		${CMAKE_CURRENT_LIST_DIR}/FileNames.h

		# Logging
		${CMAKE_CURRENT_LIST_DIR}/Logging.h
		${CMAKE_CURRENT_LIST_DIR}/Logging.cpp

		# Application class
		${CMAKE_CURRENT_LIST_DIR}/Application.h
		${CMAKE_CURRENT_LIST_DIR}/Application.cpp

		# GLFW wrapper class
		${CMAKE_CURRENT_LIST_DIR}/GLFWWrapper.h
		${CMAKE_CURRENT_LIST_DIR}/GLFWWrapper.cpp

		# Layer classes
		${CMAKE_CURRENT_LIST_DIR}/Layer.h
		${CMAKE_CURRENT_LIST_DIR}/Layer.cpp
		${CMAKE_CURRENT_LIST_DIR}/LayerStack.h
		${CMAKE_CURRENT_LIST_DIR}/LayerStack.cpp

		# Input functions
		${CMAKE_CURRENT_LIST_DIR}/Input.h
		${CMAKE_CURRENT_LIST_DIR}/Input.cpp

		# Timestep class
		${CMAKE_CURRENT_LIST_DIR}/Timestep.h
)
