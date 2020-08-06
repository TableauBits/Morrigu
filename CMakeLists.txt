cmake_minimum_required(VERSION 3.15)
project(Morrigu CXX)

# glad doesn't support configurations other than Debug and Release, so we enforce that only these two are generated
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CONAN_SYSTEM_INCLUDES ON)

include("${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake")
conan_basic_setup()

file(GLOB_RECURSE MORRIGU_SRC "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable("${PROJECT_NAME}" "${MORRIGU_SRC}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY CXX_STANDARD 17)

target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/src/Morrigu")
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/src/Sandbox")
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/src/Vendor")

if (MSVC)
    message("DETECTED MSVC COMPILER")
    # the permissive flag is only supported in VS2017+, so please delete this line if your version is not new enough,
    # although the codebase is in C++17, so the compiler you're using is probably supporting it :p
    target_compile_options("${PROJECT_NAME}" PRIVATE /permissive- /W4 /WX)
    # setting the default project in the solution
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")
else()
    message("DETECTED NON MSVC COMPILER")
    # we consider that the majority of C++ compilers other than MSVC recognise the following flags,
    # otherwise feel free to modify them
    target_compile_options("${PROJECT_NAME}" PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Werror)
endif()

add_compile_definitions(GLFW_INCLUDE_NONE IMGUI_IMPL_OPENGL_LOADER_GLAD)
conan_target_link_libraries("${PROJECT_NAME}")