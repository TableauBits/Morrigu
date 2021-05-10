find_program(GLSLC glslc)

include(${CMAKE_CURRENT_LIST_DIR}/ColoredMesh/CMakeLists.txt)

foreach (SHADER ${SHADER_SOURCES})
	message(STATUS "Shader ${SHADER} added to dependencies")
	get_filename_component(FILE_NAME ${SHADER} NAME)
	set(SPIRV_OUTPUT ${CMAKE_SOURCE_DIR}/runtime/shaders/${FILE_NAME}.spv)
	add_custom_command(
			OUTPUT ${SPIRV_OUTPUT}
			COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_OUTPUT}
			DEPENDS ${SHADER}
	)
	set(
			SPIRV_BINARIES
			${SPIRV_BINARIES}
			${SPIRV_OUTPUT}
	)
endforeach ()

add_custom_target(
		Shaders
		DEPENDS ${SPIRV_BINARIES}
)
