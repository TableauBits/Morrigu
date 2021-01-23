#ifndef MRG_CLASS_EVENT
#define MRG_CLASS_EVENT

#include "Core/Core.h"

#include <functional>
#include <ostream>
#include <string>

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
		KeyTyped,
		// Mouse events
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = MRG_BIT(0),
		EventCategoryInput = MRG_BIT(1),
		EventCategoryKeyboard = MRG_BIT(2),
		EventCategoryMouse = MRG_BIT(3),
		EventCategoryMouseButton = MRG_BIT(4)
	};

#define MRG_EVENT_CLASS_TYPE(type)                                                                                                         \
	[[nodiscard]] static EventType getStaticType() { return EventType::type; }                                                             \
	[[nodiscard]] virtual EventType getEventType() const override { return getStaticType(); }                                              \
	[[nodiscard]] virtual const char* getName() const override { return #type; }

#define MRG_EVENT_CLASS_CATEGORY(category)                                                                                                 \
	[[nodiscard]] virtual int getCategoryFlags() const override { return category; }

	class Event
	{
	public:
		virtual ~Event() = default;

		bool handled = false;

		[[nodiscard]] virtual EventType getEventType() const = 0;
		[[nodiscard]] virtual const char* getName() const = 0;
		[[nodiscard]] virtual int getCategoryFlags() const = 0;
		[[nodiscard]] virtual std::string toString() const { return getName(); }

		[[nodiscard]] bool isInCategory(EventCategory category) const { return getCategoryFlags() & category; }
	};

	class EventDispatcher
	{
	public:
		explicit EventDispatcher(Event& event) : m_event(event) {}

		template<typename T, typename F>
		bool dispatch(const F& function)
		{
			if (m_event.getEventType() == T::getStaticType()) {
				m_event.handled |= function(static_cast<T&>(m_event));
				return true;
			}
			return false;
		}

	private:
		Event& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event) { return os << event.toString(); }

}  // namespace MRG

#endif
