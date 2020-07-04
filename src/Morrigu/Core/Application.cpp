#include "Application.h"

#include "GLFW/glfw3.h"

#include <functional>

namespace MRG
{
	Application::Application()
	{
		m_window = std::make_unique<Window>(WindowProperties{"Morrigu", 1280, 720, true});
		m_window->setEventCallback(MRG_EVENT_BIND_FUNCTION(Application::onEvent));
	}
	Application::~Application() { glfwTerminate(); }

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>(MRG_EVENT_BIND_FUNCTION(Application::onWindowClose));

		MRG_ENGINE_TRACE("Event received: {0}", event.toString());
	}

	void Application::run()
	{
		while (m_running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}
}  // namespace MRG