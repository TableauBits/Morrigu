#ifndef MRG_CLASS_EVENT
#define MRG_CLASS_EVENT

#include "Core/Core.h"

namespace MRG
{
	enum class EventType
	{
		None = 0,
		// Windows Events
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		// App events
		AppTick,
		AppUpdate,
		AppRender,
		// Key events
		KeyPressed,
		KeyReleased,
		// Mouse events
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMove,
		MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	class Event
	{};
}  // namespace MRG

#endif