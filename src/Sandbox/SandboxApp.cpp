#include "../Morrigu/Events/ApplicationEvent.h"
#include <Morrigu.h>

class Sandbox : public MRG::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void run() override
	{
		MRG::WindowResizeEvent e(1280, 720);
		if (e.isInCategory(MRG::EventCategoryApplication))
			MRG_TRACE(e.toString());
		MRG_CORE_ASSERT(e.getWidth() == 1280, "Nique");
		while (true) {}
	};
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }