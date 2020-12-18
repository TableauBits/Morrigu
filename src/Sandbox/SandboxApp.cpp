#include "Sandbox2D.h"

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new Macha{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }
