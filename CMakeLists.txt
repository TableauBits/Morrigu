cmake_minimum_required(VERSION 3.15)

project(Morrigu CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
	if (ENABLE_BUILD_WITH_TIME_TRACE)
		target_compile_options(project_options INTERFACE -ftime-trace)
	endif ()
endif ()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" OFF)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)

# define conan required libraries
set(
		CONAN_REQUIRES
		spdlog/1.8.5
		glm/0.9.9.8
		glfw/3.3.2
		imgui/1.82
		stb/20200203
		tinyobjloader/1.0.6
		vulkan-headers/1.2.170.0
		vk-bootstrap/0.2
		vulkan-memory-allocator/2.3.0
)

include(cmake/Conan.cmake)
run_conan()

add_subdirectory(src)
