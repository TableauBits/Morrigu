# TODO: replace with sub directories cmake lists
file(GLOB_RECURSE MAIN_SRC ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.c)

add_library(${MAIN_PRJ_NAME} ${MAIN_SRC})
set_property(TARGET ${MAIN_PRJ_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${MAIN_PRJ_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${MAIN_PRJ_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Vendor)
