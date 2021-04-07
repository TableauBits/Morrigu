set(
		MRG_EVENT_SOURCES

		# Base event class
		${CMAKE_CURRENT_LIST_DIR}/Event.h

		# Derivative event classes
		${CMAKE_CURRENT_LIST_DIR}/ApplicationEvent.h
		${CMAKE_CURRENT_LIST_DIR}/KeyEvent.h
		${CMAKE_CURRENT_LIST_DIR}/MouseEvent.h
)