//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>

struct GradientUniform
{
	alignas(16) glm::vec3 colorA;
	alignas(16) glm::vec3 colorB;
};

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override
	{
		mainCamera.position    = {0.f, 0.f, -3.f};
		mainCamera.aspectRatio = 1280.f / 720.f;
		mainCamera.setPerspective(glm::radians(70.f), 0.1f, 200.f);
		mainCamera.recalculateViewProjection();

		const auto brickTexture = application->renderer.createTexture("brick.jpg");
		const auto leafTexture  = application->renderer.createTexture("leaf.jpg");
		const auto textureBlendingShader =
		  application->renderer.createShader("TextureBlendingShader.vert.spv", "TextureBlendingShader.frag.spv");
		const auto textureBlendingMaterial = application->renderer.createMaterial<MRG::TexturedVertex>(textureBlendingShader);
		textureBlendingMaterial->bindTexture(0, leafTexture);
		textureBlendingMaterial->bindTexture(1, brickTexture);

		m_suzanne = MRG::RenderObject<MRG::TexturedVertex>::create(MRG::Utils::loadMeshFromFileTexturedVertex("monkey_smooth.obj"),
		                                                           textureBlendingMaterial);
		m_boxy =
		  MRG::RenderObject<MRG::TexturedVertex>::create(MRG::Utils::loadMeshFromFileTexturedVertex("boxy.obj"), textureBlendingMaterial);

		m_suzanne->translate({1.5f, 0.f, 0.f});
		m_suzanne->rotate({0.f, 1.f, 0.f}, glm::radians(180.f));
		m_suzanne->setVisible(false);
		m_boxy->translate({-1.5f, -1.5f, 0.f});

		application->renderer.uploadMesh(m_suzanne->mesh);
		application->renderer.uploadMesh(m_boxy->mesh);

		renderables.emplace_back(m_suzanne->getRenderData());
		renderables.emplace_back(m_boxy->getRenderData());
	}

	void onDetach() override { MRG_INFO("my final message. goodb ye") }
	void onUpdate(MRG::Timestep ts) override
	{
		m_elapsedTime += ts;
		application->renderer.clearColor.b = std::abs(std::sin(m_elapsedTime * 3.14f));
		m_suzanne->rotate({0.f, 1.f, 0.f}, ts.getSeconds() * glm::radians(180.f));
		m_boxy->rotate({0.f, 1.f, 0.f}, ts.getSeconds() * glm::radians(180.f));
	}
	void onEvent(MRG::Event& event) override
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>([this](MRG::WindowResizeEvent& event) { return mainCamera.onResize(event); });
		dispatcher.dispatch<MRG::KeyReleasedEvent>([this](MRG::KeyReleasedEvent&) {
			m_suzanne->setVisible(false);
			return true;
		});
		dispatcher.dispatch<MRG::KeyPressedEvent>([this](MRG::KeyPressedEvent&) {
			m_suzanne->setVisible(true);
			return true;
		});
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>> m_suzanne{}, m_boxy{};
	float m_elapsedTime{};
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{.windowName            = "Morrigu sample app",
	                                                     .rendererSpecification = {.applicationName      = "SampleApp",
	                                                                               .windowWidth          = 1280,
	                                                                               .windowHeight         = 720,
	                                                                               .preferredPresentMode = vk::PresentModeKHR::eMailbox}})
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
