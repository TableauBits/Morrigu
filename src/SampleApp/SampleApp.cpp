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

		const auto shader   = createShader("TestShader.vert.spv", "TestShader.frag.spv");
		const auto material = createMaterial<MRG::TexturedVertex>(shader);

		auto circleMesh = MRG::Utils::Meshes::circle<MRG::TexturedVertex>();
		uploadMesh(circleMesh);
		m_circle = createRenderObject(circleMesh, material);

		renderables.emplace_back(m_circle->getRenderData());

		auto* materialEditor = new MaterialEditorLayer;
		materialEditor->setMaterial(material);
		application->pushLayer(materialEditor);
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>> m_circle{};
	float m_lerpTValue = 0.f;
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
