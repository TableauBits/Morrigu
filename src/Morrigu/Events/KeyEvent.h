#ifndef MRG_CLASS_KEYEVENT
#define MRG_CLASS_KEYEVENT

#include "Core/Input.h"
#include "Events/Event.h"

#include <sstream>

namespace MRG
{
	class KeyEvent : public Event
	{
	public:
		[[nodiscard]] KeyCode getKeyCode() const { return m_keyCode; }

		MRG_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(KeyCode keyCode) : m_keyCode(keyCode) {}

		KeyCode m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keyCode, unsigned int repeatCount) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}

		[[nodiscard]] unsigned int getRepeatCount() const { return m_repeatCount; }
		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_repeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keyCode;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(KeyTyped)
	};
}  // namespace MRG

#endif