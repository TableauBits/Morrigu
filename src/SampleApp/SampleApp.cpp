//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override { MRG_INFO("Hello from SampleLayer!") }
	void onDetach() override { MRG_INFO("my final message. goodb ye") }
	void onUpdate(MRG::Timestep) override {}
	void onEvent(const MRG::Event&) override {}
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{
	        .windowName            = "Morrigu sample app",
	        .rendererSpecification = {
	          .applicationName = "SampleApp", .windowWidth = 1280, .windowHeight = 720, .presentMode = VK_PRESENT_MODE_FIFO_KHR}})
	{
		pushLayer(new SampleLayer());
	};
};

int main() {
    MRG::Logger::init();
    SampleApp application{};

    application.init();
    application.run();
    application.cleanup();
}
