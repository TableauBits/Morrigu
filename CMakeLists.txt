cmake_minimum_required(VERSION 3.18.1)
project(Morrigu CXX)

set(MAIN_PRJ_NAME ${PROJECT_NAME})
set(TEST_PRJ_NAME Sandbox)

# glad doesn't support configurations other than Debug and Release, so we enforce that only these two are generated
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
# This line ensure that cmake considers conan includes as system includes, which should not generate warnings
set(CONAN_SYSTEM_INCLUDES ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup()

add_compile_definitions(GLFW_INCLUDE_NONE GLFW_INCLUDE_VULKAN IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(src/${MAIN_PRJ_NAME})
add_subdirectory(src/${TEST_PRJ_NAME})

if (MSVC)
    message("DETECTED MSVC COMPILER")
    # the permissive flag is only supported in VS2017+, so please delete this line if your version is not new enough,
    # although the codebase is in C++17, so the compiler you're using is probably supporting it :p
    target_compile_options(${MAIN_PRJ_NAME} PRIVATE /permissive- /W4 /WX)
    target_compile_options(${TEST_PRJ_NAME} PRIVATE /permissive- /W4 /WX)

    # this line removes the console from the windows build. You probably want to uncomment this for a release build.
    ## target_link_options(${TEST_PRJ_NAME} PRIVATE /SUBSYSTEM:windows /ENTRY:mainCRTStartup)
    
    # setting the default project in the solution
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_PRJ_NAME})
    set_property(TARGET ${TEST_PRJ_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime)
else()
    message("DETECTED NON MSVC COMPILER")
    # we consider that the majority of C++ compilers other than MSVC recognise the following flags,
    # otherwise feel free to modify them
    target_compile_options(${MAIN_PRJ_NAME} PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Werror)
    target_compile_options(${TEST_PRJ_NAME} PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Werror)
endif()

# Finding and adding vulkan to the list of libraries
find_package(Vulkan REQUIRED)
set(CONAN_LIBS_DEBUG ${CONAN_LIBS_DEBUG} ${Vulkan_LIBRARIES})
set(CONAN_LIBS_RELEASE ${CONAN_LIBS_RELEASE} ${Vulkan_LIBRARIES})

target_include_directories(${MAIN_PRJ_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(${TEST_PRJ_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

conan_target_link_libraries(${MAIN_PRJ_NAME})
