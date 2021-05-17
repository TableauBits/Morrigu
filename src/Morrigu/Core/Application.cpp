//
// Created by Mathis Lamidey on 2021-04-03.
//

#include "Application.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include <utility>

namespace
{
	void glfwErrorCallback(int errorCode, const char* description) { MRG_ENGINE_ERROR("GLFW error #{}: \"{}\"", errorCode, description) }
}  // namespace

namespace MRG
{
	Application::Application(ApplicationSpecification spec) : m_specification(std::move(spec)) { init(); }
	Application::~Application()
	{
		m_renderer.cleanup();
		glfwTerminate();
	}

	void Application::run()
	{
		while (m_isRunning) {
			const auto time = static_cast<float>(glfwGetTime());
			Timestep ts{time - m_lastTime};
			m_lastTime = time;

			m_renderer.beginFrame();
			for (auto& layer : m_layers) { layer->onUpdate(ts); }
			m_renderer.endFrame();

			glfwPollEvents();
		}
	}

	void Application::pushLayer(Layer* newLayer)
	{
		newLayer->application = this;
		m_layers.pushLayer(newLayer);
	}

	Layer* Application::popLayer() { return m_layers.popLayer(); }

	void Application::setWindowTitle(const char* title) const { glfwSetWindowTitle(m_renderer.window, title); }
	void Application::uploadMesh(Mesh& mesh) { m_renderer.uploadMesh(mesh); }
	void Application::drawMesh(const Mesh& mesh, const Camera& camera) { m_renderer.drawMesh(mesh, camera); }

	void Application::init()
	{
		[[maybe_unused]] const auto result = glfwInit();
		MRG_ENGINE_ASSERT(result == GLFW_TRUE, "failed to initialise GLFW!")

		glfwSetErrorCallback(glfwErrorCallback);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		auto window = glfwCreateWindow(m_specification.rendererSpecification.windowWidth,
		                               m_specification.rendererSpecification.windowHeight,
		                               m_specification.windowName.c_str(),
		                               nullptr,
		                               nullptr);

		glfwSetWindowUserPointer(window, this);

		// set up callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			application->m_specification.rendererSpecification.windowWidth  = width;
			application->m_specification.rendererSpecification.windowHeight = height;

			auto resize = WindowResizeEvent(width, height);
			application->onEvent(resize);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			auto close = WindowCloseEvent();
			application->onEvent(close);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int keycode, int, int action, int) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			switch (action) {
			case GLFW_PRESS:
			case GLFW_REPEAT: {
				auto keyPress = KeyPressedEvent(static_cast<KeyCode>(keycode), 0);  // @TODO(Ithyx): make repeatcount work
				application->onEvent(keyPress);
			} break;
			case GLFW_RELEASE: {
				auto keyRelease = KeyReleasedEvent(static_cast<KeyCode>(keycode));
				application->onEvent(keyRelease);
			} break;

			default:
				MRG_ENGINE_WARN("Ignored unknown GLFW keyboard event {}", action)
			}
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			switch (action) {
			case GLFW_PRESS: {
				auto mousePress = MouseButtonPressedEvent(static_cast<MouseCode>(button));
				application->onEvent(mousePress);
			} break;
			case GLFW_RELEASE: {
				auto mouseRelease = MouseButtonReleasedEvent(static_cast<MouseCode>(button));
				application->onEvent(mouseRelease);
			} break;

			default:
				MRG_ENGINE_WARN("Ignored unknown GLFW mouse event {}", action)
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			auto mouseScroll = MouseScrolledEvent(static_cast<float>(xOffset), static_cast<float>(yOffset));
			application->onEvent(mouseScroll);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			auto mouseMove = MouseMovedEvent(static_cast<float>(xPos), static_cast<float>(yPos));
			application->onEvent(mouseMove);
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, std::uint32_t codePoint) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(window));

			auto keyType = KeyTypedEvent(codePoint);
			application->onEvent(keyType);
		});

		// callbacks set, now give ownership of window to renderer
		m_renderer.init(m_specification.rendererSpecification, window);
	}

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) { return onClose(event); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) { return onResize(event); });

		for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
			if (event.handled) { break; }
			(*it)->onEvent(event);
		}
	}

	bool Application::onClose(WindowCloseEvent&)
	{
		m_isRunning = false;
		return true;
	}

	bool Application::onResize(WindowResizeEvent& resize)
	{
		int width = resize.getWidth(), height = resize.getHeight();
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(m_renderer.window, &width, &height);
			glfwWaitEvents();
		}

		m_renderer.spec.windowWidth  = width;
		m_renderer.spec.windowHeight = height;
		m_renderer.onResize();

		return false;
	}
}  // namespace MRG
