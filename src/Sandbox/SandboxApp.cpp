#include "Sandbox2D.h"

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new SandboxLayer{}); }
	~Sandbox() override = default;
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }
