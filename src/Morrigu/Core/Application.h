#ifndef MRG_CLASS_APPLICATION
#define MRG_CLASS_APPLICATION

#include "Events/ApplicationEvent.h"
#include "Window.h"

namespace MRG
{
	class Application
	{
	private:
	public:
		Application();
		virtual ~Application();

		void onEvent(Event& event);
		void run();

	private:
		bool onWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_window;
		bool m_running = true;
	};
	Application* createApplication();
}  // namespace MRG

#endif