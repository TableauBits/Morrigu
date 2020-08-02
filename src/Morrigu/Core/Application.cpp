#include "Application.h"

#include "Renderer/Renderer.h"

#include <functional>

namespace MRG
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		MRG_CORE_ASSERT(s_instance == nullptr, "Application already exists !");
		s_instance = this;

		m_window = std::make_unique<Window>(WindowProperties{"Morrigu", 1280, 720, true});
		m_window->setEventCallback([this](Event& event) { onEvent(event); });

		m_ImGuiLayer = new ImGuiLayer{};
		pushOverlay(m_ImGuiLayer);

		// triangle
		// clang-format off
		static constexpr float triangleVertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		
		std::shared_ptr<VertexBuffer> triangleVertexBuffer;
		triangleVertexBuffer.reset(VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));

		triangleVertexBuffer->layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};
		// clang-format on
		m_triangleArray.reset(VertexArray::create());
		m_triangleArray->addVertexBuffer(triangleVertexBuffer);

		unsigned int indices[3] = {0, 1, 2};
		std::shared_ptr<IndexBuffer> triangleIndexBuffer;
		triangleIndexBuffer.reset(IndexBuffer::create(indices, 3));
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
		
		std::shared_ptr<VertexBuffer> squareVertexBuffer;
		squareVertexBuffer.reset(VertexBuffer::create(squareVertices, sizeof(squareVertices)));

		squareVertexBuffer->layout = {
			{ShaderDataType::Float3, "a_Position"}
		};
		// clang-format on
		m_squareArray.reset(VertexArray::create());
		m_squareArray->addVertexBuffer(squareVertexBuffer);

		uint32_t squareindices[6] = {0, 1, 2, 2, 3, 0};
		std::shared_ptr<IndexBuffer> squareIndexBuffer;
		squareIndexBuffer.reset(IndexBuffer::create(squareindices, 6));
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
		m_triangleShader.reset(new Shader(triangleVShader, triangleFShader));

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
		m_squareShader.reset(new Shader(squareVShader, squareFShader));
	}

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) -> bool { return onWindowClose(event); });

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(event);
			if (event.handled)
				break;
		}
	}

	void Application::run()
	{
		while (m_running) {
			RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
			RenderCommand::clear();

			m_squareShader->bind();
			Renderer::submit(m_squareArray);

			m_triangleShader->bind();
			Renderer::submit(m_triangleArray);

			for (auto& layer : m_layerStack) layer->onUpdate();

			m_ImGuiLayer->begin();
			for (auto& layer : m_layerStack) layer->onImGuiRender();
			m_ImGuiLayer->end();
			m_window->onUpdate();
		}
	}

	void Application::pushLayer(Layer* newLayer) { m_layerStack.pushLayer(newLayer); }
	void Application::pushOverlay(Layer* newOverlay) { m_layerStack.pushOverlay(newOverlay); }

	bool Application::onWindowClose([[maybe_unused]] WindowCloseEvent& event)
	{
		m_running = false;
		return true;
	}
}  // namespace MRG