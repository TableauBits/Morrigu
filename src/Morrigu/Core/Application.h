#ifndef MRG_CLASS_APPLICATION
#define MRG_CLASS_APPLICATION

namespace MRG
{
	class Application
	{
	private:
	public:
		Application();
		virtual ~Application();

		virtual void run() = 0;
	};
	Application* createApplication();
}  // namespace MRG

#endif