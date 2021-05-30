include(${CMAKE_CURRENT_LIST_DIR}/Utils/CMakeLists.txt)

set(
		SA_SOURCES

		# Main file
		${CMAKE_CURRENT_LIST_DIR}/SampleApp.cpp

		# Utils files
		${SA_UTILS_SOURCES}
)
