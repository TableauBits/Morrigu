#include "Application.h"

#include "Debug/Instrumentor.h"
#include "Renderer/Renderer.h"

#include <GLFW/glfw3.h>

#include <functional>

namespace MRG
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(s_instance == nullptr, "Application already exists !");
		s_instance = this;

		m_window = std::make_unique<Window>(WindowProperties::create("Morrigu", 1280, 720, true));
		m_window->setEventCallback([this](Event& event) { onEvent(event); });

		Renderer::init();

		m_ImGuiLayer = new ImGuiLayer{};
		pushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		MRG_PROFILE_FUNCTION();

		Renderer::shutdown();
	}

	void Application::onEvent(Event& event)
	{
		MRG_PROFILE_FUNCTION();

		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& closeEvent) -> bool { return onWindowClose(closeEvent); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) -> bool { return onWindowResize(resizeEvent); });

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it) {
			(*it)->onEvent(event);
			if (event.handled)
				break;
		}
	}

	void Application::run()
	{
		MRG_PROFILE_FUNCTION();

		while (m_running) {
			MRG_PROFILE_SCOPE("RunLoop");

			auto time = float(glfwGetTime());
			Timestep ts = time - m_lastFrameTime;
			m_lastFrameTime = time;

			if (!m_minimized) {
				MRG_PROFILE_SCOPE("LayerStack onUpdate");

				for (auto& layer : m_layerStack) layer->onUpdate(ts);
			}

			m_ImGuiLayer->begin();
			{
				MRG_PROFILE_SCOPE("LayerStack onImGuiRender");

				for (auto& layer : m_layerStack) layer->onImGuiRender();
			}
			m_ImGuiLayer->end();
			m_window->onUpdate();
		}
	}

	void Application::pushLayer(Layer* newLayer)
	{
		MRG_PROFILE_FUNCTION();

		m_layerStack.pushLayer(newLayer);
	}
	void Application::pushOverlay(Layer* newOverlay)
	{
		MRG_PROFILE_FUNCTION();

		m_layerStack.pushOverlay(newOverlay);
	}

	bool Application::onWindowClose([[maybe_unused]] WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event)
	{
		MRG_PROFILE_FUNCTION();

		if (event.getWidth() == 0 || event.getHeight() == 0) {
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::onWindowResize(event.getWidth(), event.getHeight());
		return false;
	}
}  // namespace MRG