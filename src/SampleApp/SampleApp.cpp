//
// Created by Mathis Lamidey on 2021-04-03.
//

#include "MaterialEditorLayer.h"

#include <Morrigu.h>

class SampleLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		mainCamera.position    = {0.f, 0.f, 3.f};
		mainCamera.aspectRatio = 1280.f / 720.f;
		mainCamera.setPerspective(glm::radians(90.f), 0.1f, 200.f);
		mainCamera.recalculateViewProjection();

		const auto testShader   = createShader("GradientShader.vert.spv", "GradientShader.frag.spv");
		const auto testMaterial = createMaterial<MRG::TexturedVertex>(testShader);

		m_testQuad = createRenderObject(MRG::Utils::Meshes::quad<MRG::TexturedVertex>(), testMaterial);
		m_suzanne  = createRenderObject(MRG::Utils::Meshes::loadMeshFromFile<MRG::TexturedVertex>("monkey_smooth.obj"),
                                       application->renderer.defaultTexturedMaterial);
		m_boxy     = createRenderObject(MRG::Utils::Meshes::loadMeshFromFile<MRG::TexturedVertex>("boxy.obj"), testMaterial);

		m_suzanne->translate({1.5f, 0.f, 0.f});
		m_suzanne->setVisible(false);
		m_boxy->translate({-1.5f, -1.5f, 0.f});
		m_boxy->setVisible(false);
		m_boxy->rotate({0.f, 1.f, 0.f}, glm::radians(90.f));

		uploadMesh(m_testQuad->mesh);
		uploadMesh(m_suzanne->mesh);
		uploadMesh(m_boxy->mesh);

		renderables.emplace_back(m_testQuad->getRenderData());
		renderables.emplace_back(m_suzanne->getRenderData());
		renderables.emplace_back(m_boxy->getRenderData());

		auto materialEditor = new MaterialEditorLayer;
		application->pushLayer(materialEditor);
		materialEditor->setMaterial(testMaterial);
	}

	void onUpdate(MRG::Timestep ts) override
	{
		mainCamera.yaw += glm::radians(ts.getSeconds() * 100);
		mainCamera.recalculateViewProjection();
	}

	void onEvent(MRG::Event& event) override
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>([this](MRG::WindowResizeEvent& event) { return mainCamera.onResize(event); });
		dispatcher.dispatch<MRG::KeyReleasedEvent>([this](MRG::KeyReleasedEvent&) {
			m_suzanne->setVisible(false);
			m_boxy->setVisible(false);
			m_testQuad->setVisible(true);
			return true;
		});
		dispatcher.dispatch<MRG::KeyPressedEvent>([this](MRG::KeyPressedEvent&) {
			m_suzanne->setVisible(true);
			m_boxy->setVisible(true);
			m_testQuad->setVisible(false);
			return true;
		});
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>> m_suzanne{}, m_boxy{}, m_testQuad{};
};

class UILayer : public MRG::UILayer
{
public:
	void onAttach() override
	{
		MRG::UILayer::onAttach();

		auto font = createFont("test.ttf");
		m_text    = createText("TEXT", font);

		m_testUI = createRenderObject(MRG::Utils::Meshes::quad<MRG::TexturedVertex>(), application->renderer.defaultUITexturedMaterial);

		m_testUI->translate({-1.f, -1.f, 0.f});
		m_testUI->scale({0.2f, 0.2f, 0.2f});

		uploadMesh(m_testUI->mesh);

		renderables.emplace_back(m_testUI->getRenderData());

		const auto textRD = m_text->getCollectionRenderData();
		for (const auto& renderData : textRD) { renderables.emplace_back(renderData); }
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>> m_testUI{};
	MRG::Ref<MRG::UI::Text> m_text;
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{
	        .windowName            = "Morrigu sample app",
	        .rendererSpecification = {.applicationName      = "SampleApp",
	                                  .windowWidth          = 1280,
	                                  .windowHeight         = 720,
	                                  .preferredPresentMode = vk::PresentModeKHR::eMailbox},
	      })
	{
		pushLayer(new SampleLayer());
		pushLayer(new UILayer());
	};
};

int main()
{
	MRG::Logger::init();
	SampleApp application{};

	application.run();
}
