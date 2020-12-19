#ifndef MRG_CLASS_MOUSEEVENT
#define MRG_CLASS_MOUSEEVENT

#include "Core/MouseCodes.h"
#include "Events/Event.h"

#include <sstream>

namespace MRG
{
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y) : m_mouseX(x), m_mouseY(y) {}

		[[nodiscard]] float getX() const { return m_mouseX; }
		[[nodiscard]] float getY() const { return m_mouseY; }

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(MouseMoved)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouseX, m_mouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float x, const float y) : m_mouseX(x), m_mouseY(y) {}

		[[nodiscard]] float getX() const { return m_mouseX; }
		[[nodiscard]] float getY() const { return m_mouseY; }

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_mouseX << ", " << m_mouseY;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(MouseScrolled)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouseX, m_mouseY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		[[nodiscard]] MouseCode getMouseButton() const { return m_button; }

		MRG_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(const MouseCode button) : m_button(button) {}

		MouseCode m_button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}  // namespace MRG

#endif
