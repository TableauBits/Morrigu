//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override { MRG_INFO("Hello from SampleLayer!") }
	void onDetach() override {}
	void onUpdate(MRG::Timestep) override {}
	void onEvent(const MRG::Event&) override {}
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{
	        .name = "SampleApp", .rendererSpecification = {.windowName = "SampleApp", .windowWidth = 1280, .windowHeight = 720}})
	{
		pushLayer(new SampleLayer());
	};
};

MRG::Application* MRG::createApplication() { return new SampleApp(); }
