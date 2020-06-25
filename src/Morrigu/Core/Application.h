#ifndef MRG_CLASS_APPLICATION
#define MRG_CLASS_APPLICATION

#include "Window.h"

namespace MRG
{
	class Application
	{
	private:
	public:
		Application();
		virtual ~Application();

		virtual void run();

	private:
		std::unique_ptr<Window> m_window;
		bool m_running = true;
	};
	Application* createApplication();
}  // namespace MRG

#endif