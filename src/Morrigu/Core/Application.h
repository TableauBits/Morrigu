#ifndef MRG_CLASS_APPLICATION
#define MRG_CLASS_APPLICATION

#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer/Buffers.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Window.h"

namespace MRG
{
	class Application
	{
	private:
	public:
		Application();
		virtual ~Application() = default;

		void onEvent(Event& event);
		void run();

		void pushLayer(Layer* newLayer);
		void pushOverlay(Layer* newOverlay);

		[[nodiscard]] inline Window& getWindow() const { return *m_window; }
		[[nodiscard]] inline static Application& get() { return *s_instance; }

	private:
		bool onWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;
		static Application* s_instance;

		std::shared_ptr<VertexArray> m_vertexArray;
		std::shared_ptr<Shader> m_shader;
	};
	Application* createApplication();
}  // namespace MRG

#endif