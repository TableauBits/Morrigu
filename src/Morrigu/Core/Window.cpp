#include "Window.h"
#include "Logger.h"

namespace MRG
{
	bool Window::s_GLFWInit = false;

	Window::Window(const WindowProperties& props) { _init(props); }
	Window::~Window() { _shutdown(); }

	void Window::_init(const WindowProperties& props)
	{
		m_properties = props;

		MRG_ENGINE_INFO("Creating window {0} ({1}x{2})", props.title, props.width, props.height);

		if (!s_GLFWInit) {
			auto success = glfwInit();
			MRG_CORE_ASSERT(success, "Could not initialize GLFW !");

			s_GLFWInit = true;
		}

		m_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_properties);

		setVsync(props.VSync);
	}

	void Window::_shutdown() { glfwDestroyWindow(m_window); }

	void Window::onUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_window);
	}

	void Window::setVsync(bool enabled)
	{
		if (enabled) {
			glfwSwapInterval(1);
		} else {
			glfwSwapInterval(0);
		}

		m_properties.VSync = enabled;
	}

}  // namespace MRG