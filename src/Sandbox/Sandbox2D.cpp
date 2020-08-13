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

void Sandbox2D::onAttach()
{
	m_vertexArray = MRG::VertexArray::create();
	// clang-format off
    float vertices[3*4] = {
        -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
    };

	auto vertexBuffer = MRG::VertexBuffer::create(vertices, sizeof(vertices));
	vertexBuffer->layout = {
        {MRG::ShaderDataType::Float3, "a_position"}
    };
    m_vertexArray->addVertexBuffer(vertexBuffer);
	// clang-format on

	uint32_t indices[] = {0, 1, 2, 2, 3, 0};
	auto indexBuffer = MRG::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
	m_vertexArray->setIndexBuffer(indexBuffer);

	m_shader = MRG::Shader::create("resources/sandbox/shaders/flatColor.glsl");
}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(MRG::Timestep ts)
{
	m_camera.onUpdate(ts);
	MRG::RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	MRG::RenderCommand::clear();

	MRG::Renderer::beginScene(m_camera.getCamera());

	std::static_pointer_cast<MRG::OpenGL::Shader>(m_shader)->bind();
	std::static_pointer_cast<MRG::OpenGL::Shader>(m_shader)->uploadUniform("u_color", m_color);

	MRG::Renderer::submit(m_shader, m_vertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	MRG::Renderer::endScene();
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Shader color:", glm::value_ptr(m_color));
	ImGui::End();
}

void Sandbox2D::onEvent(MRG::Event& event) { m_camera.onEvent(event); }