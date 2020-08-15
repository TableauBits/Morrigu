# TODO: replace with sub directories cmake lists
file(GLOB_RECURSE TEST_SRC ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(${TEST_PRJ_NAME} ${TEST_SRC})
target_link_libraries(${TEST_PRJ_NAME} PRIVATE ${PROJECT_NAME})

set_property(TARGET ${TEST_PRJ_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${TEST_PRJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
