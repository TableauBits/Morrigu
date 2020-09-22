#include "Sandbox2D.h"

class VulkanSandbox : public MRG::Layer
{
public:
	VulkanSandbox() : Layer("Vulkan layer") {}

	void onAttach() override {}
	void onDetach() override {}
	void onUpdate(MRG::Timestep) override {}
	void onEvent(MRG::Event&) override {}
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new Sandbox2D{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }