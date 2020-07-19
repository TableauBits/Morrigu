#include <Morrigu.h>

#include "imgui.h"

class SampleLayer : public MRG::Layer
{
public:
	SampleLayer() : Layer("Sample Layer") {}

	void onAttach() override {}
	void onDetach() override {}
	void onUpdate() override
	{
		if (MRG::Input::isKeyDown(GLFW_KEY_SPACE))
			MRG_TRACE("UPDATE !");
	}
	void onEvent(MRG::Event& event) override { MRG_TRACE("{}", event.toString()); }
	void onImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello world !");
		ImGui::End();
	}
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new SampleLayer{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }