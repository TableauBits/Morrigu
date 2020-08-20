#include "Application.h"

#include "Renderer/Renderer.h"

#include <GLFW/glfw3.h>

#include <functional>

namespace MRG
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		MRG_CORE_ASSERT(s_instance == nullptr, "Application already exists !");
		s_instance = this;

		m_window = std::make_unique<Window>(WindowProperties{"Morrigu", 1280, 720, true});
		m_window->setEventCallback([this](Event& event) { onEvent(event); });

		Renderer::init();

		m_ImGuiLayer = new ImGuiLayer{};
		pushOverlay(m_ImGuiLayer);
	}

	Application::~Application() { Renderer::shutdown(); }

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& closeEvent) -> bool { return onWindowClose(closeEvent); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) -> bool { return onWindowResize(resizeEvent); });

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(event);
			if (event.handled)
				break;
		}
	}

	void Application::run()
	{
		while (m_running) {
			auto time = float(glfwGetTime());
			Timestep ts = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized) {
				for (auto& layer : m_layerStack) layer->onUpdate(ts);
			}

			m_ImGuiLayer->begin();
			for (auto& layer : m_layerStack) layer->onImGuiRender();
			m_ImGuiLayer->end();
			m_window->onUpdate();
		}
	}

	void Application::pushLayer(Layer* newLayer) { m_layerStack.pushLayer(newLayer); }
	void Application::pushOverlay(Layer* newOverlay) { m_layerStack.pushOverlay(newOverlay); }

	bool Application::onWindowClose([[maybe_unused]] WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event)
	{
		if (event.getWidth() == 0 || event.getHeight() == 0) {
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::onWindowResize(event.getWidth(), event.getHeight());
		return false;
	}
}  // namespace MRG