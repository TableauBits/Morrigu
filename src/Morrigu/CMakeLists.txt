# TODO: replace with sub directories cmake lists
file(GLOB_RECURSE MAIN_SRC ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(${MAIN_PRJ_NAME} ${MAIN_SRC})
set_property(TARGET ${MAIN_PRJ_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${MAIN_PRJ_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${MAIN_PRJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Vendor)

if (MSVC)
    message(DETECTED MSVC COMPILER)
    # the permissive flag is only supported in VS2017+, so please delete this line if your version is not new enough,
    # although the codebase is in C++17, so the compiler you're using is probably supporting it :p
    target_compile_options(${MAIN_PRJ_NAME} PRIVATE /permissive- /W4 /WX)
else()
    message(DETECTED NON MSVC COMPILER)
    # we consider that the majority of C++ compilers other than MSVC recognise the following flags,
    # otherwise feel free to modify them
    target_compile_options(${MAIN_PRJ_NAME} PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Werror)
endif()
