#include "../Morrigu/Events/ApplicationEvent.h"
#include <Morrigu.h>

class Sandbox : public MRG::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }