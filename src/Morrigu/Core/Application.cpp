#include "Application.h"

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

		m_ImGuiLayer = new ImGuiLayer{};
		pushOverlay(m_ImGuiLayer);
	}

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) -> bool { return onWindowClose(event); });

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(event);
			if (event.handled)
				break;
		}
	}

	void Application::run()
	{
		while (m_running) {
			for (auto& layer : m_layerStack) layer->onUpdate();

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
}  // namespace MRG