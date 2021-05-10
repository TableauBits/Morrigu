//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override
	{
		m_triangleMesh.vertices.resize(3);
		m_triangleMesh.vertices[0].position = {1.f, 1.f, 0.f};
		m_triangleMesh.vertices[1].position = {-1.f, 1.f, 0.f};
		m_triangleMesh.vertices[2].position = {0.f, -1.f, 0.f};

		m_triangleMesh.vertices[0].color = {0.f, 1.f, 0.f};
		m_triangleMesh.vertices[1].color = {0.f, 1.f, 0.f};
		m_triangleMesh.vertices[2].color = {0.f, 1.f, 0.f};

		application->uploadMesh(m_triangleMesh);
	}
	void onDetach() override { MRG_INFO("my final message. goodb ye") }
	void onUpdate(MRG::Timestep) override { application->drawMesh(m_triangleMesh); }
	void onEvent(const MRG::Event&) override {}

private:
	MRG::Mesh m_triangleMesh;
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

int main()
{
	MRG::Logger::init();
	SampleApp application{};

	application.run();
}
