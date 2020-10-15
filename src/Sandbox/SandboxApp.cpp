#include "Sandbox2D.h"

class VulkanSandbox : public MRG::Layer
{
public:
	VulkanSandbox() : Layer("Vulkan layer"), m_camera(1280.f / 720.f) {}

	void onAttach() override {}
	void onDetach() override {}
	void onUpdate(MRG::Timestep ts) override
	{
		m_camera.onUpdate(ts);

		if (MRG::Input::isKeyDown(MRG::Key::Space))
			MRG_INFO("FPS: {}", 1 / ts);

		MRG::Renderer2D::beginScene(m_camera.getCamera());
		MRG::Renderer2D::drawQuad({0, 0, 0}, {0, 0}, {0, 0, 0, 0});
		MRG::Renderer2D::endScene();
	}
	void onEvent(MRG::Event&) override {}

private:
	MRG::OrthoCameraController m_camera;
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new VulkanSandbox{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }