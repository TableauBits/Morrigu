#ifndef MRG_CLASS_APPLICATION
#define MRG_CLASS_APPLICATION

#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "LayerStack.h"
#include "Window.h"

int main();

namespace MRG
{
	class Application
	{
	private:
	public:
		explicit Application(const char* name = "Morrigu App");
		virtual ~Application();

		void onEvent(Event& event);

		void pushLayer(Layer* newLayer);
		void pushOverlay(Layer* newOverlay);

		void close();

		[[nodiscard]] Window& getWindow() const { return *m_window; }
		[[nodiscard]] ImGuiLayer* getImGuiLayer() const { return m_ImGuiLayer; }
		[[nodiscard]] static Application& get() { return *s_instance; }

	private:
		void run();

		bool onWindowClose(WindowCloseEvent& event);
		bool onWindowResize(WindowResizeEvent& event);

		Scope<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;

		static Application* s_instance;

		friend int ::main();
	};
	Application* createApplication();
}  // namespace MRG

#endif
