#include "Renderer2D.h"

#include "Renderer/APIs/OpenGL/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace MRG
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> flatColorShader;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::init()
	{
		s_data = new Renderer2DStorage;
		s_data->quadVertexArray = VertexArray::create();

		// clang-format off
        float squareVertices[3 * 4] = {
            -0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
        };

		Ref<VertexBuffer> squareVB = VertexBuffer::create(squareVertices, sizeof(squareVertices));
        squareVB->layout = {
            { ShaderDataType::Float3, "a_position" }
        };
		// clang-format on
		s_data->quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
		Ref<IndexBuffer> squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		s_data->quadVertexArray->setIndexBuffer(squareIB);

		s_data->flatColorShader = Shader::create("resources/sandbox/shaders/flatColor.glsl");
	}

	void Renderer2D::shutdown() { delete s_data; }

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		std::static_pointer_cast<OpenGL::Shader>(s_data->flatColorShader)->bind();
		std::static_pointer_cast<OpenGL::Shader>(s_data->flatColorShader)
		  ->uploadUniform("u_viewProjection", camera.getProjectionViewMatrix());
		std::static_pointer_cast<OpenGL::Shader>(s_data->flatColorShader)->uploadUniform("u_transform", glm::mat4(1.0f));
	}

	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(glm::vec3(position, 1.0f), size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const glm::vec4& color)
	{
		std::static_pointer_cast<OpenGL::Shader>(s_data->flatColorShader)->bind();
		std::static_pointer_cast<OpenGL::Shader>(s_data->flatColorShader)->uploadUniform("u_color", color);

		s_data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_data->quadVertexArray);
	}
}  // namespace MRG