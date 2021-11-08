//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_APPLICATIONEVENT_H
#define MORRIGU_APPLICATIONEVENT_H

#include "Events/Event.h"

#include <sstream>

namespace MRG
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(int width, int height) : m_width(width), m_height(height) {}

		[[nodiscard]] int getWidth() const { return m_width; }
		[[nodiscard]] int getHeight() const { return m_height; }

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		MRG_EVENT_CLASS_TYPE(WindowResize)
		MRG_EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		int m_width, m_height;
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

#endif  // MORRIGU_APPLICATIONEVENT_H
