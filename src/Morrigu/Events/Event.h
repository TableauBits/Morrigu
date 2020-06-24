#ifndef MRG_CLASS_EVENT
#define MRG_CLASS_EVENT

#include "Core/Core.h"

#include <functional>
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
		// Mouse events
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
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

#define MRG_EVENT_CLASS_TYPE(type)                                                                                                         \
	[[nodiscard]] static EventType getStaticType() { return EventType::type; }                                                             \
	[[nodiscard]] virtual EventType getEventType() const override { return getStaticType(); }                                              \
	[[nodiscard]] virtual const char* getName() const override { return #type; }

#define MRG_EVENT_CLASS_CATEGORY(category)                                                                                                 \
	[[nodiscard]] virtual int getCategoryFlags() const override { return category; }

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool isInCategory(EventCategory category) { return getCategoryFlags() & category; }

	protected:
		bool m_handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFunction = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : m_event(event) {}

		template<typename T>
		bool dispatch(EventFunction<T> function)
		{
			if (m_event.getEventType() == T::getStaticType()) {
				m_event.m_handled = function(*(T*)&m_event);
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