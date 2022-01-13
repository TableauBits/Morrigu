include(${CMAKE_CURRENT_LIST_DIR}/LightCasters/CMakeLists.txt)

set(
		MC_COMPONENT_SOURCES

		# Light Casters
		${MC_LIGHT_CASTERS_SOURCES}

		# Entity settings component
		${CMAKE_CURRENT_LIST_DIR}/EntitySettings.h
)
