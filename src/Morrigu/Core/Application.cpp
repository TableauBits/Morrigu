#include "Application.h"

#include <glad/glad.h>

#include <functional>

namespace MRG
{
	Application* Application::s_instance = nullptr;

	// THIS FUNCTION NEEDS TO BE MOVED SOMEWHERE
	// Where ? I have no idea :)
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		// clang-format off
		switch (type)
		{
			case MRG::ShaderDataType::Float:  return GL_FLOAT;
			case MRG::ShaderDataType::Float2: return GL_FLOAT;
			case MRG::ShaderDataType::Float3: return GL_FLOAT;
			case MRG::ShaderDataType::Float4: return GL_FLOAT;

			case MRG::ShaderDataType::Mat3:   return GL_FLOAT;
			case MRG::ShaderDataType::Mat4:   return GL_FLOAT;

			case MRG::ShaderDataType::Int:    return GL_INT;
			case MRG::ShaderDataType::Int2:   return GL_INT;
			case MRG::ShaderDataType::Int3:   return GL_INT;
			case MRG::ShaderDataType::Int4:   return GL_INT;

			case MRG::ShaderDataType::Bool:   return GL_BOOL;
		}
		// clang-format on

		MRG_CORE_ASSERT(false, "Invalid shader data type ! ({})", type);
		return 0;
	}

	Application::Application()
	{
		MRG_CORE_ASSERT(s_instance == nullptr, "Application already exists !");
		s_instance = this;

		m_window = std::make_unique<Window>(WindowProperties{"Morrigu", 1280, 720, true});
		m_window->setEventCallback([this](Event& event) { onEvent(event); });

		m_ImGuiLayer = new ImGuiLayer{};
		pushOverlay(m_ImGuiLayer);

		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);

		// clang-format off
		static constexpr float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		m_vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		m_vertexBuffer->layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};
		// clang-format on

		auto index = 0;
		const auto& layout = m_vertexBuffer->layout;
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
			                      element.getComponentCount(),
			                      ShaderDataTypeToOpenGLBaseType(element.type),
			                      element.isNormalized ? GL_TRUE : GL_FALSE,
			                      layout.getStride(),
			                      (const void*)(element.offset));
			++index;
		}

		unsigned int indices[3] = {0, 1, 2};
		m_indexBuffer.reset(IndexBuffer::create(indices, 3));

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
			glBindVertexArray(m_vertexArray);
			glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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