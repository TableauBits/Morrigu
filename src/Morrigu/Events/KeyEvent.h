#ifndef MRG_CLASS_KEYEVENT
#define MRG_CLASS_KEYEVENT

#include "Event.h"

#include <sstream>

namespace MRG
{
	class KeyEvent : public Event
	{
	public:
		[[nodiscard]] inline int getKeyCode() const { return m_keyCode; }

		MRG_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keyCode) : m_keyCode(keyCode) {}

		int m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}

		[[nodiscard]] inline int getRepeatCount() const { return m_repeatCount; }
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
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(KeyReleased)
	};
}  // namespace MRG

#endif