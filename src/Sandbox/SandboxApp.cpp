#include <Morrigu.h>

class SampleLayer : public MRG::Layer
{
public:
	SampleLayer() : Layer("Sample Layer") {}

	void onAttach() override {}
	void onDetach() override {}
	void onUpdate() override { MRG_TRACE("UPDATE !"); }
	void onEvent(MRG::Event& event) override { MRG_TRACE("{}", event.toString()); }
};

class Sandbox : public MRG::Application
{
public:
	Sandbox()
	{
		pushLayer(new SampleLayer{});
		pushLayer(new MRG::ImGuiLayer{});
	}
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }