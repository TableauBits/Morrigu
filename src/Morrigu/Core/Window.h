#ifndef MRG_CLASS_WINDOW
#define MRG_CLASS_WINDOW

#include "Events/Event.h"

#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
#include <string>

namespace MRG
{
	using EventCallbackFunction = std::function<void(Event&)>;
	struct WindowProperties
	{
		std::string title;
		unsigned int width;
		unsigned int height;
		bool VSync;
		EventCallbackFunction callback;
		unsigned int keyRepeats;
	};

	class Window
	{
	public:
		Window(const WindowProperties& props);
		~Window();

		void onUpdate();

		[[nodiscard]] unsigned int getWidth() const { return m_properties.width; }
		[[nodiscard]] unsigned int getHeight() const { return m_properties.height; }
		[[nodiscard]] bool isVsync() const { return m_properties.VSync; }

		void setEventCallback(const EventCallbackFunction& callback) { m_properties.callback = callback; }
		void setVsync(bool enabled);

		GLFWwindow* getGLFWWindow() const { return m_window; }

	private:
		void _init(const WindowProperties& props);
		void _shutdown();

		GLFWwindow* m_window;
		WindowProperties m_properties;

		static bool s_GLFWInit;
	};
}  // namespace MRG

#endif