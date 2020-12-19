#ifndef MRG_CLASS_KEYEVENT
#define MRG_CLASS_KEYEVENT

#include "Core/KeyCodes.h"
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
		KeyEvent(const KeyCode keyCode) : m_keyCode(keyCode) {}

		KeyCode m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keyCode, const uint16_t repeatCount) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}

		[[nodiscard]] uint16_t getRepeatCount() const { return m_repeatCount; }
		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(KeyPressed)

	private:
		uint16_t m_repeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

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
		KeyTypedEvent(const KeyCode keyCode) : KeyEvent(keyCode) {}

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
