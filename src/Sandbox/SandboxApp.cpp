#include "Sandbox2D.h"

class VulkanSandbox : public MRG::Layer
{
public:
	VulkanSandbox() : Layer("Vulkan layer") {}

	void onAttach() override {}
	void onDetach() override {}
	void onUpdate(MRG::Timestep) override
	{
		// MRG::Renderer2D::beginScene(MRG::OrthoCamera(0, 0, 0, 0));
		MRG::Renderer2D::drawQuad({0, 0, 0}, {0, 0}, {0, 0, 0, 0});
		// MRG::Renderer2D::endScene();
	}
	void onEvent(MRG::Event&) override {}
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new VulkanSandbox{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }