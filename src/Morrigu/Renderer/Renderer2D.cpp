#include "Renderer2D.h"

#include "Renderer/APIs/OpenGL/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace MRG
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> flatColorShader;
		Ref<Shader> textureShader;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::init()
	{
		s_data = new Renderer2DStorage;
		s_data->quadVertexArray = VertexArray::create();

		// clang-format off
        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.f, 0.f,
			 0.5f, -0.5f, 0.0f, 1.f, 0.f,
			 0.5f,  0.5f, 0.0f, 1.f, 1.f,
			-0.5f,  0.5f, 0.0f, 0.f, 1.f
        };

		Ref<VertexBuffer> squareVB = VertexBuffer::create(squareVertices, sizeof(squareVertices));
        squareVB->layout = {
            { ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float2, "a_texCoord" }
        };
		// clang-format on
		s_data->quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
		Ref<IndexBuffer> squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		s_data->quadVertexArray->setIndexBuffer(squareIB);

		s_data->flatColorShader = Shader::create("resources/sandbox/shaders/flatColor.glsl");
		s_data->textureShader = Shader::create("resources/sandbox/shaders/texture.glsl");
		s_data->textureShader->bind();
		s_data->textureShader->upload("u_texture", 0);
	}

	void Renderer2D::shutdown() { delete s_data; }

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		s_data->flatColorShader->bind();
		s_data->flatColorShader->upload("u_viewProjection", camera.getProjectionViewMatrix());

		s_data->textureShader->bind();
		s_data->textureShader->upload("u_viewProjection", camera.getProjectionViewMatrix());
	}

	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(glm::vec3(position, 1.0f), size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_data->flatColorShader->bind();
		s_data->flatColorShader->upload("u_color", color);

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		s_data->flatColorShader->upload("u_transform", transform);

		s_data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_data->quadVertexArray);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		drawQuad(glm::vec3(position, 1.0f), size, texture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		s_data->textureShader->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		s_data->textureShader->upload("u_transform", transform);

		texture->bind();

		s_data->quadVertexArray->bind();
		RenderCommand::drawIndexed(s_data->quadVertexArray);
	}
}  // namespace MRG