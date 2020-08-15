#include "Sandbox2D.h"

#include "Core/Warnings.h"

// This is very temporary. I don't know how to properly deal with that for now,
// so this will have to do.
DISABLE_WARNING_PUSH
DISABLE_WARNING_NONSTANDARD_EXTENSION
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNING_POP
#include "Renderer/APIs/OpenGL/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

Sandbox2D::Sandbox2D() : MRG::Layer("Sandbox 2D"), m_camera(1280.f / 720.f) {}

void Sandbox2D::onAttach() {}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(MRG::Timestep ts)
{
	m_camera.onUpdate(ts);
	MRG::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	MRG::RenderCommand::clear();

	MRG::Renderer2D::beginScene(m_camera.getCamera());
	MRG::Renderer2D::drawQuad({0.f, 0.f}, {1.f, 1.f}, {0.8f, 0.2f, 0.3f, 1.f});
	MRG::Renderer2D::endScene();
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Shader color:", glm::value_ptr(m_color));
	ImGui::End();
}

void Sandbox2D::onEvent(MRG::Event& event) { m_camera.onEvent(event); }