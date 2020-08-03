#include <Morrigu.h>

#include "imgui.h"

class SampleLayer : public MRG::Layer
{
public:
	SampleLayer() : Layer("Sample Layer"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f) {}

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
		
		std::shared_ptr<MRG::VertexBuffer> triangleVertexBuffer;
		triangleVertexBuffer.reset(MRG::VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));

		triangleVertexBuffer->layout = {
			{MRG::ShaderDataType::Float3, "a_Position"},
			{MRG::ShaderDataType::Float4, "a_Color"}
		};
		// clang-format on
		m_triangleArray.reset(MRG::VertexArray::create());
		m_triangleArray->addVertexBuffer(triangleVertexBuffer);

		unsigned int indices[3] = {0, 1, 2};
		std::shared_ptr<MRG::IndexBuffer> triangleIndexBuffer;
		triangleIndexBuffer.reset(MRG::IndexBuffer::create(indices, 3));
		m_triangleArray->setIndexBuffer(triangleIndexBuffer);

		// square
		// clang-format off
		static constexpr float squareVertices[3 * 4] =
		{
			-0.40f, -0.40f, 0.0f,
			 0.40f, -0.40f, 0.0f,
			 0.40f,  0.40f, 0.0f,
			-0.40f,  0.40f, 0.0f
		};
		
		std::shared_ptr<MRG::VertexBuffer> squareVertexBuffer;
		squareVertexBuffer.reset(MRG::VertexBuffer::create(squareVertices, sizeof(squareVertices)));

		squareVertexBuffer->layout = {
			{MRG::ShaderDataType::Float3, "a_Position"}
		};
		// clang-format on
		m_squareArray.reset(MRG::VertexArray::create());
		m_squareArray->addVertexBuffer(squareVertexBuffer);

		uint32_t squareindices[6] = {0, 1, 2, 2, 3, 0};
		std::shared_ptr<MRG::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer.reset(MRG::IndexBuffer::create(squareindices, 6));
		m_squareArray->setIndexBuffer(squareIndexBuffer);

		std::string triangleVShader = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_viewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_viewProjection * vec4(a_Position, 1.0);
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
		m_triangleShader.reset(new MRG::Shader(triangleVShader, triangleFShader));

		std::string squareVShader = R"(
			#version 460 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_viewProjection;

			void main()
			{
				gl_Position = u_viewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string squareFShader = R"(
			#version 460 core

			layout(location = 0) out vec4 color;
			
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);;
			}
		)";
		m_squareShader.reset(new MRG::Shader(squareVShader, squareFShader));
	}
	void onDetach() override {}
	void onUpdate(MRG::Timestep ts) override
	{
		if (MRG::Input::isKeyDown(GLFW_KEY_SPACE))
			MRG_TRACE("Framerate: {} ({} milliseconds)", 1.0f / ts.getSeconds(), ts.getMillieconds());

		static const float movSpeed = 2.f;
		static const float rotSpeed = 180.f;
		if (MRG::Input::isKeyDown(GLFW_KEY_W))
			position.y += movSpeed * ts;

		if (MRG::Input::isKeyDown(GLFW_KEY_A))
			position.x -= movSpeed * ts;

		if (MRG::Input::isKeyDown(GLFW_KEY_S))
			position.y -= movSpeed * ts;

		if (MRG::Input::isKeyDown(GLFW_KEY_D))
			position.x += movSpeed * ts;

		if (MRG::Input::isKeyDown(GLFW_KEY_Q))
			rotation -= rotSpeed * ts;

		if (MRG::Input::isKeyDown(GLFW_KEY_E))
			rotation += rotSpeed * ts;

		MRG::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		MRG::RenderCommand::clear();

		m_camera.setPosition(glm::vec3(position, 0.0f));
		m_camera.setRotation(rotation);

		MRG::Renderer::beginScene(m_camera);

		MRG::Renderer::submit(m_squareShader, m_squareArray);
		MRG::Renderer::submit(m_triangleShader, m_triangleArray);

		MRG::Renderer::endScene();
	}
	void onEvent(MRG::Event&) override {}
	void onImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello world !");
		ImGui::End();
	}

private:
	std::shared_ptr<MRG::VertexArray> m_triangleArray;
	std::shared_ptr<MRG::Shader> m_triangleShader;

	std::shared_ptr<MRG::VertexArray> m_squareArray;
	std::shared_ptr<MRG::Shader> m_squareShader;

	MRG::OrthoCamera m_camera;
	glm::vec2 position = {0.0f, 0.0f};
	float rotation = 0.0f;
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new SampleLayer{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }