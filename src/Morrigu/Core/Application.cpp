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
	Application::Application(ApplicationSpecification spec) : m_specification(std::move(spec))
	{
		glfwSetErrorCallback(glfwErrorCallback);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		auto window = glfwCreateWindow(m_specification.rendererSpecification.windowWidth,
		                               m_specification.rendererSpecification.windowHeight,
		                               m_specification.windowName.c_str(),
		                               nullptr,
		                               nullptr);

		glfwSetWindowUserPointer(window, this);

		// set up callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* eventWindow, int width, int height) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			application->m_specification.rendererSpecification.windowWidth  = width;
			application->m_specification.rendererSpecification.windowHeight = height;

			auto resize = WindowResizeEvent(width, height);
			application->onEvent(resize);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* eventWindow) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			auto close = WindowCloseEvent();
			application->onEvent(close);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* eventWindow, int keycode, int, int action, int) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			switch (action) {
			case GLFW_PRESS:
			case GLFW_REPEAT: {
				auto keyPress = KeyPressedEvent(keycode, 0);  // @TODO(Ithyx): make repeatcount work
				application->onEvent(keyPress);
			} break;
			case GLFW_RELEASE: {
				auto keyRelease = KeyReleasedEvent(keycode);
				application->onEvent(keyRelease);
			} break;

			default:
				MRG_ENGINE_WARN("Ignored unknown GLFW keyboard event {}", action)
			}
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* eventWindow, int button, int action, int) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			switch (action) {
			case GLFW_PRESS: {
				auto mousePress = MouseButtonPressedEvent(button);
				application->onEvent(mousePress);
			} break;
			case GLFW_RELEASE: {
				auto mouseRelease = MouseButtonReleasedEvent(button);
				application->onEvent(mouseRelease);
			} break;

			default:
				MRG_ENGINE_WARN("Ignored unknown GLFW mouse event {}", action)
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* eventWindow, double xOffset, double yOffset) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			auto mouseScroll = MouseScrolledEvent(static_cast<float>(xOffset), static_cast<float>(yOffset));
			application->onEvent(mouseScroll);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* eventWindow, double xPos, double yPos) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			auto mouseMove = MouseMovedEvent(static_cast<float>(xPos), static_cast<float>(yPos));
			application->onEvent(mouseMove);
		});

		glfwSetCharCallback(window, [](GLFWwindow* eventWindow, std::uint32_t codePoint) {
			auto application = static_cast<Application*>(glfwGetWindowUserPointer(eventWindow));

			auto keyType = KeyTypedEvent(static_cast<KeyCode>(codePoint));
			application->onEvent(keyType);
		});

		// callbacks set, now give ownership of window to renderer
		renderer = createScope<Renderer>(m_specification.rendererSpecification, window);
	}

	void Application::run()
	{
		while (m_isRunning) {
			const auto time = static_cast<float>(glfwGetTime());
			Timestep ts{time - m_lastTime};
			elapsedTime += ts;
			renderer->elapsedTime = elapsedTime;
			m_lastTime            = time;

			renderer->beginFrame();
			for (auto& layer : m_layers) { layer->onUpdate(ts); }
			renderer->beginImGui();
			for (auto& layer : m_layers) { layer->onImGuiUpdate(ts); }
			renderer->endImGui();
			renderer->endFrame();

			glfwPollEvents();
		}
	}

	void Application::pushLayer(Layer* newLayer)
	{
		newLayer->application = this;
		m_layers.pushLayer(newLayer);
	}

	Layer* Application::popLayer() { return m_layers.popLayer(); }

	void Application::setWindowTitle(const char* title) const { glfwSetWindowTitle(renderer->window, title); }
	void Application::close() { m_isRunning = false; }

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& closeEvent) { return onClose(closeEvent); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) { return onResize(resizeEvent); });

		for (auto& m_layer : std::ranges::reverse_view(m_layers)) {
			if (event.handled) { break; }
			m_layer->onEvent(event);
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
			glfwGetFramebufferSize(renderer->window, &width, &height);
			glfwWaitEvents();
		}

		renderer->spec.windowWidth  = width;
		renderer->spec.windowHeight = height;
		renderer->onResize();

		return false;
	}
}  // namespace MRG
