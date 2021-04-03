macro(run_conan)
	# Download automatically, you can also just copy the conan.cmake file
	if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
		message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
		file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
	endif ()

	include(${CMAKE_BINARY_DIR}/conan.cmake)

	conan_cmake_run(
			REQUIRES
			${CONAN_EXTRA_REQUIRES}
			glm/0.9.9.8
			glfw/3.3.2
			imgui/1.82
			stb/20200203
			tinyobjloader/1.0.6
			vulkan-headers/1.2.170.0
			vk-bootstrap/0.2
			vulkan-memory-allocator/2.3.0
			OPTIONS
			${CONAN_EXTRA_OPTIONS}
			BASIC_SETUP
			CMAKE_TARGETS # individual targets to link to
			BUILD
			missing)
endmacro()
