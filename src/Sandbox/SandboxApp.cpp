#include <Morrigu.h>

#include "imgui.h"

class SampleLayer : public MRG::Layer
{
public:
	SampleLayer() : Layer("Sample Layer") {}

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

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
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

			void main()
			{
				gl_Position = vec4(a_Position, 1.0);
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
	void onUpdate() override
	{
		MRG::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		MRG::RenderCommand::clear();

		m_squareShader->bind();
		MRG::Renderer::submit(m_squareArray);

		m_triangleShader->bind();
		MRG::Renderer::submit(m_triangleArray);

		if (MRG::Input::isKeyDown(GLFW_KEY_SPACE))
			MRG_TRACE("UPDATE !");
	}
	void onEvent(MRG::Event& event) override { MRG_TRACE("{}", event.toString()); }
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
};

class Sandbox : public MRG::Application
{
public:
	Sandbox() { pushLayer(new SampleLayer{}); }
	~Sandbox() {}
};

MRG::Application* MRG::createApplication() { return new Sandbox(); }