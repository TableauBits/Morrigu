cmake_minimum_required(VERSION 3.15)
project(Morrigu CXX)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
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
    add_compile_options(/permissive- /W4 /w14640)
elseif()
    # we consider that the majority of C++ compilers other than MSVC recognise the following flags,
    # otherwise feel free to modify them
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
endif()

target_link_libraries("${PROJECT_NAME}" PRIVATE ${CONAN_LIBS})