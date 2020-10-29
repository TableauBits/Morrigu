#include "Sandbox2D.h"

Sandbox2D::Sandbox2D() : MRG::Layer("Sandbox 2D"), m_camera(1280.f / 720.f) {}

void Sandbox2D::onAttach()
{
	MRG_PROFILE_FUNCTION();

	m_checkerboard = MRG::Texture2D::create("resources/textures/Checkerboard.png");
	m_character = MRG::Texture2D::create("resources/textures/Character.png");
	m_camera.movementSpeed = 2.f;
}

void Sandbox2D::onDetach() { MRG_PROFILE_FUNCTION(); }

void Sandbox2D::onUpdate(MRG::Timestep ts)
{
	MRG_PROFILE_FUNCTION();

	m_camera.onUpdate(ts);

	if (MRG::Input::isKeyDown(MRG::Key::Space))
		MRG_INFO("FPS: {}", 1 / ts);

	{
		MRG_PROFILE_SCOPE("Render prep");
		MRG::Renderer2D::setClearColor(m_color);
	}

	{
		MRG_PROFILE_SCOPE("Render draw");
		MRG::Renderer2D::beginScene(m_camera.getCamera());
		MRG::Renderer2D::drawRotatedQuad({-1.f, 0.f}, {0.8f, 0.8f}, glm::radians(-45.f), {0.8f, 0.2f, 0.3f, 1.f});
		MRG::Renderer2D::drawQuad({0.f, 0.f, -0.1f}, {10.f, 10.f}, m_checkerboard, 10.f);
		MRG::Renderer2D::drawQuad({0.5f, -0.5f}, {1.f, 1.f}, m_character, 1.f);
		MRG::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender()
{
	MRG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Shader color:", glm::value_ptr(m_color));
	ImGui::End();
}

void Sandbox2D::onEvent(MRG::Event& event) { m_camera.onEvent(event); }