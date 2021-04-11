include("${CMAKE_CURRENT_LIST_DIR}/Core/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Events/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Rendering/CMakeLists.txt")

set(
		MRG_SOURCES

		# Global include file
		${CMAKE_CURRENT_LIST_DIR}/Morrigu.h

		# Core files
		${MRG_CORE_SOURCES}

		# Event files
		${MRG_EVENT_SOURCES}

		# Rendering files
		${MRG_RENDERING_SOURCES}
)