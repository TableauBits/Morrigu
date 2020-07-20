#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Logger.h"

#include "GLFW/glfw3.h"

#include <glad/glad.h>

namespace MRG
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		MRG_ENGINE_ERROR("GLFW error detected: {0}: {1}", error, description);
	}

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
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInit = true;
		}

		m_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);

		int status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
		MRG_CORE_ASSERT(status, "Could not initialize glad !");

		glfwSetWindowUserPointer(m_window, &m_properties);

		setVsync(props.VSync);

		// GLFW callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
			data->width = width;
			data->height = height;

			WindowResizeEvent resize{static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
			data->callback(resize);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			WindowCloseEvent close{};
			data->callback(close);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent press{keycode, 0};
				data->callback(press);
			} break;

			case GLFW_REPEAT: {
				KeyPressedEvent press{keycode, ++data->keyRepeats};
				data->callback(press);
			} break;

			case GLFW_RELEASE: {
				data->keyRepeats = 0;
				KeyReleasedEvent release{keycode};
				data->callback(release);
			} break;

			default: {
				MRG_ENGINE_WARN("Ignoring unrecognized GLFW key event (type {})", action);
			} break;
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent press{button};
				data->callback(press);
			} break;

			case GLFW_RELEASE: {
				MouseButtonReleasedEvent release{button};
				data->callback(release);
			} break;

			default: {
				MRG_ENGINE_WARN("Ignoring unrecognized GLFW mouse event (type {})", action);
			} break;
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			MouseScrolledEvent scroll{static_cast<float>(xOffset), static_cast<float>(yOffset)};
			data->callback(scroll);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent move{static_cast<float>(xPos), static_cast<float>(yPos)};
			data->callback(move);
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codePoint) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			KeyTypedEvent typedChar{static_cast<int>(codePoint)};
			data->callback(typedChar);
		});
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