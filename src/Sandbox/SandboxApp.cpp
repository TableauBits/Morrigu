#include <Morrigu.h>

class Sandbox : public MRG::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void run() override
	{
		while (true) {}
	};
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }