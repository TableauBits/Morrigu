//
// Created by Mathis Lamidey on 2021-04-03.
//

#include "MaterialEditorLayer.h"

#include <Morrigu.h>
#include <imgui.h>

class SampleLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		mainCamera->aspectRatio = 16.f / 9.f;
		mainCamera->position    = {0.f, 0.f, 1.f};
		mainCamera->setPerspective(90, 0.001f, 1000.f);
		mainCamera->recalculateViewProjection();

		basicCamera.aspectRatio = 16.f / 9.f;
		basicCamera.setOrthgraphic(1, -1.f, 1.f);
		basicCamera.recalculateViewProjection();

		const auto shader   = createShader("TestShader.vert.spv", "TestShader.frag.spv");
		const auto material = createMaterial<MRG::TexturedVertex>(shader);

		auto circleMesh = MRG::Utils::Meshes::circle<MRG::TexturedVertex>();
		uploadMesh(circleMesh);
		m_circle = createRenderObject(circleMesh, material);

		m_fbDrawables.emplace_back(m_circle->getRenderData());

		auto quadMesh = MRG::Utils::Meshes::quad<MRG::TexturedVertex>();
		uploadMesh(quadMesh);
		m_quad = createRenderObject(quadMesh, application->renderer->defaultTexturedMaterial);
		m_quad->scale({16.f / 9.f, 1.f, 1.f});

		m_basicDrawables.emplace_back(m_quad->getRenderData());

		auto* materialEditor = new MaterialEditorLayer;
		materialEditor->setMaterial(material);
		application->pushLayer(materialEditor);

		m_framebuffer = createFramebuffer({
		  .width  = 1280,
		  .height = 720,
		});

		m_quad->bindTexture(1, m_framebuffer);
	}

	void onUpdate(MRG::Timestep) override
	{
		application->renderer->draw(m_fbDrawables.begin(), m_fbDrawables.end(), *mainCamera, m_framebuffer);
		application->renderer->draw(m_basicDrawables.begin(), m_basicDrawables.end(), basicCamera);
	}

	void onEvent(MRG::Event& event) override
	{
		MRG::EventDispatcher dispatcher{event};

		dispatcher.dispatch<MRG::WindowResizeEvent>(
		  [this](MRG::WindowResizeEvent& resizeEvent) { return basicCamera.onResize(resizeEvent); });
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>> m_circle{}, m_quad{};
	std::vector<MRG::RenderData> m_fbDrawables, m_basicDrawables;
	MRG::Ref<MRG::Framebuffer> m_framebuffer;
	float m_lerpTValue = 0.f;
	MRG::StandardCamera basicCamera{};
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application{MRG::ApplicationSpecification{.windowName            = "Sample Application",
	                                                     .rendererSpecification = {
	                                                       .applicationName      = "Sample MRG Application",
	                                                       .windowWidth          = 1280,
	                                                       .windowHeight         = 720,
	                                                       .preferredPresentMode = vk::PresentModeKHR::eMailbox,
	                                                     }}}
	{
		pushLayer(new SampleLayer);
	}
};

int main()
{
	MRG::Logger::init();
	SampleApp app{};

	app.run();
}
