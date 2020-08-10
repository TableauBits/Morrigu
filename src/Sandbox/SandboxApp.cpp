#include <Morrigu.h>

#include "Core/Warnings.h"

// This is very temporary. I don't know how to properly deal with that for now,
// so this will have to do.
DISABLE_WARNING_PUSH
DISABLE_WARNING_NONSTANDARD_EXTENSION
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNING_POP
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

// VERY TEMPORARY
#include "Renderer/APIs/OpenGL/Shader.h"

class SampleLayer : public MRG::Layer
{
public:
	SampleLayer() : Layer("Sample Layer"), m_camera(1280.f / 720.f, true) {}

	void onAttach() override
	{
		// triangle
		// clang-format off
		static constexpr float triangleVertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		
		MRG::Ref<MRG::VertexBuffer> triangleVertexBuffer;
		triangleVertexBuffer = MRG::VertexBuffer::create(triangleVertices, sizeof(triangleVertices));

		triangleVertexBuffer->layout = {
			{MRG::ShaderDataType::Float3, "a_Position"},
			{MRG::ShaderDataType::Float4, "a_Color"}
		};
		// clang-format on
		m_triangleArray = MRG::VertexArray::create();
		m_triangleArray->addVertexBuffer(triangleVertexBuffer);

		unsigned int indices[3] = {0, 1, 2};
		MRG::Ref<MRG::IndexBuffer> triangleIndexBuffer;
		triangleIndexBuffer = MRG::IndexBuffer::create(indices, 3);
		m_triangleArray->setIndexBuffer(triangleIndexBuffer);

		// square
		// clang-format off
		static constexpr float squareVertices[5 * 4] =
		{
			-0.50f, -0.50f, 0.0f, 0.0f, 0.0f,
			 0.50f, -0.50f, 0.0f, 1.0f, 0.0f,
			 0.50f,  0.50f, 0.0f, 1.0f, 1.0f,
			-0.50f,  0.50f, 0.0f, 0.0f, 1.0f 
		};
		
		MRG::Ref<MRG::VertexBuffer> squareVertexBuffer;
		squareVertexBuffer = MRG::VertexBuffer::create(squareVertices, sizeof(squareVertices));

		squareVertexBuffer->layout = {
			{MRG::ShaderDataType::Float3, "a_Position"},
			{MRG::ShaderDataType::Float2, "a_texCoord"}
		};
		// clang-format on
		m_squareArray = MRG::VertexArray::create();
		m_squareArray->addVertexBuffer(squareVertexBuffer);

		uint32_t squareindices[6] = {0, 1, 2, 2, 3, 0};
		MRG::Ref<MRG::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer = MRG::IndexBuffer::create(squareindices, 6);
		m_squareArray->setIndexBuffer(squareIndexBuffer);

		std::string triangleVShader = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_viewProjection;
			uniform mat4 u_transform;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_viewProjection * u_transform * vec4(a_Position, 1.0);
			}
		)";

		std::string triangleFShader = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";
		m_triangleShader = MRG::Shader::create("triangleShader", triangleVShader, triangleFShader);

		std::string squareVShader = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_viewProjection;
			uniform mat4 u_transform;

			void main()
			{
				gl_Position = u_viewProjection * u_transform * vec4(a_Position, 1.0);
			}
		)";

		std::string squareFShader = R"(
			#version 460 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_color;
			
			void main()
			{
				color = vec4(u_color, 1.0);;
			}
		)";
		m_squareShader = MRG::Shader::create("squareShader", squareVShader, squareFShader);

		auto textureShader = m_shaderLibrary.load("resources/sandbox/shaders/texture.glsl");

		m_texture = MRG::Texture2D::create("resources/sandbox/textures/Checkerboard.png");
		m_logo = MRG::Texture2D::create("resources/sandbox/textures/Logo.png");

		std::static_pointer_cast<MRG::OpenGL::Shader>(textureShader)->bind();
		std::static_pointer_cast<MRG::OpenGL::Shader>(textureShader)->uploadUniform("u_texture", 0);
	}
	void onDetach() override {}
	void onUpdate(MRG::Timestep ts) override
	{
		if (MRG::Input::isKeyDown(GLFW_KEY_SPACE))
			MRG_TRACE("Framerate: {} ({} milliseconds)", 1.0f / ts.getSeconds(), ts.getMillieconds());

		MRG::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		MRG::RenderCommand::clear();

		m_camera.onUpdate(ts);

		MRG::Renderer::beginScene(m_camera.getCamera());

		auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::static_pointer_cast<MRG::OpenGL::Shader>(m_squareShader)->bind();
		std::static_pointer_cast<MRG::OpenGL::Shader>(m_squareShader)->uploadUniform("u_color", m_squareColor);

		for (std::size_t y = 0; y < 20; ++y) {
			for (std::size_t x = 0; x < 20; ++x) {
				glm::vec3 pos{x * 0.11f, y * 0.11f, 0.f};
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				MRG::Renderer::submit(m_squareShader, m_squareArray, transform);
			}
		}

		auto textureShader = m_shaderLibrary.get("texture");

		m_texture->bind();
		MRG::Renderer::submit(textureShader, m_squareArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_logo->bind();
		MRG::Renderer::submit(textureShader, m_squareArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// MRG::Renderer::submit(m_triangleShader, m_triangleArray);

		MRG::Renderer::endScene();
	}
	void onEvent(MRG::Event& event) override { m_camera.onEvent(event); }

	void onImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Squares color", glm::value_ptr(m_squareColor));
		ImGui::End();
	}

private:
	MRG::Ref<MRG::VertexArray> m_triangleArray;
	MRG::Ref<MRG::Shader> m_triangleShader;

	MRG::Ref<MRG::VertexArray> m_squareArray;
	MRG::Ref<MRG::Shader> m_squareShader;

	MRG::ShaderLibrary m_shaderLibrary;
	MRG::Ref<MRG::Texture2D> m_texture, m_logo;

	MRG::OrthoCameraController m_camera;

	glm::vec3 m_squareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new SampleLayer{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }