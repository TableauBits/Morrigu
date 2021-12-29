include(${CMAKE_CURRENT_LIST_DIR}/Components/CMakeLists.txt)

set(
		MRG_ENTITY_SOURCES

		# Component sources
		${MRG_COMPONENT_SOURCES}

		# Entity types
		${CMAKE_CURRENT_LIST_DIR}/Entity.h

		# Transform struct
		${CMAKE_CURRENT_LIST_DIR}/Transform.h
)
