#include "Application.h"

#include <glad/glad.h>

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

		m_vertexArray.reset(VertexArray::create());

		// clang-format off
		static constexpr float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		vertexBuffer->layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};
		// clang-format on
		m_vertexArray->addVertexBuffer(vertexBuffer);

		unsigned int indices[3] = {0, 1, 2};
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, 3));
		m_vertexArray->setIndexBuffer(indexBuffer);

		std::string vertexShader = R"(
			#version 410 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentShader = R"(
			#version 410 core

			layout(location = 0) out vec4 color;
			
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";
		m_shader.reset(new Shader(vertexShader, fragmentShader));
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
			glClearColor(0.2f, 0.2f, 0.2f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_shader->bind();
			m_vertexArray->bind();
			glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);

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