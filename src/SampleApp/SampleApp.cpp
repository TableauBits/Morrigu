//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override
	{
		m_camera.position    = {0.f, 0.f, -3.f};
		m_camera.aspectRatio = 1280.f / 720.f;
		m_camera.setPerspective(glm::radians(70.f), 0.1f, 200.f);
		m_camera.recalculateViewProjection();

		m_sampleMesh.translate({0.f, -1.5f, 0.f});

		application->uploadMesh(m_sampleMesh);
	}
	void onDetach() override { MRG_INFO("my final message. goodb ye") }
	void onUpdate(MRG::Timestep ts) override
	{
		m_sampleMesh.rotate({0.f, 1.f, 0.f}, ts.getSeconds() * glm::radians(180.f));
		application->drawMesh(m_sampleMesh, m_camera);
	}
	void onEvent(MRG::Event& event) override
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>([this](MRG::WindowResizeEvent& event) { return m_camera.onResize(event); });
	}

private:
	MRG::Camera m_camera{};
	MRG::Mesh m_sampleMesh = MRG::Mesh::loadFromFile("boxy.obj");
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{
	        .windowName            = "Morrigu sample app",
	        .rendererSpecification = {
	          .applicationName = "SampleApp", .windowWidth = 1280, .windowHeight = 720, .presentMode = vk::PresentModeKHR::eFifo}})
	{
		pushLayer(new SampleLayer());
	};
};

int main()
{
	MRG::Logger::init();
	SampleApp application{};

	application.run();
}
