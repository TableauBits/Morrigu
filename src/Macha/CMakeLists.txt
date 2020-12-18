# TODO: replace with sub directories cmake lists
file(GLOB_RECURSE EDITOR_SRC ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(${EDITOR_PRJ_NAME} ${EDITOR_SRC})
target_link_libraries(${EDITOR_PRJ_NAME} PRIVATE ${PROJECT_NAME})

set_property(TARGET ${EDITOR_PRJ_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${EDITOR_PRJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
