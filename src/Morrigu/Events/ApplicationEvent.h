#ifndef MRG_CLASS_APPLICATIONEVENT
#define MRG_CLASS_APPLICATIONEVENT

#include "Event.h"

#include <sstream>

namespace MRG
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

		[[nodiscard]] inline unsigned int getWidth() { return m_width; }
		[[nodiscard]] inline unsigned int getHeight() { return m_height; }

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(WindowResize)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_width, m_height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		MRG_EVENT_CLASS_TYPE(WindowClose)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		MRG_EVENT_CLASS_TYPE(AppTick)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		MRG_EVENT_CLASS_TYPE(AppUpdate)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		MRG_EVENT_CLASS_TYPE(AppRender)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}  // namespace MRG

#endif