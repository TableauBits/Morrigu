#include "Application.h"

#include "GLFW/glfw3.h"

namespace MRG
{
	Application::Application() { m_window = std::make_unique<Window>(WindowProperties{"Morrigu", 1280, 720, true}); }
	Application::~Application() { glfwTerminate(); }

	void Application::run()
	{
		while (m_running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->onUpdate();
		}
	}
}  // namespace MRG