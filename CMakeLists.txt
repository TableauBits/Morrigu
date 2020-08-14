cmake_minimum_required(VERSION 3.15)
project(Morrigu CXX)

set(MAIN_PRJ_NAME ${PROJECT_NAME})
set(TEST_PRJ_NAME Sandbox)

# glad doesn't support configurations other than Debug and Release, so we enforce that only these two are generated
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
# This line ensure that cmake considers conan includes as system includes, which should not generate warnings
set(CONAN_SYSTEM_INCLUDES ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup()

add_compile_definitions(GLFW_INCLUDE_NONE IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(src/Morrigu)
add_subdirectory(src/Sandbox)

if(MSVC)
    # setting the default project in the solution
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_PRJ_NAME})
    set_property(TARGET ${TEST_PRJ_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

conan_target_link_libraries(${MAIN_PRJ_NAME})
