#include "Renderer2D.h"

#include "Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace MRG::OpenGL
{
	void Renderer2D::init(GLFWwindow*)
	{
		m_quadVertexArray = VertexArray::create();

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
		m_quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
		Ref<IndexBuffer> squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		m_quadVertexArray->setIndexBuffer(squareIB);

		m_whiteTexture = Texture2D::create(1, 1);
		auto whiteTextureData = 0xffffffff;
		m_whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		m_textureShader = Shader::create("resources/shaders/texture");
		m_textureShader->bind();
		m_textureShader->upload("u_texture", 0);
	}

	void Renderer2D::shutdown() {}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		m_textureShader->bind();
		m_textureShader->upload("u_viewProjection", camera.getProjectionViewMatrix());
	}

	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		m_textureShader->upload("u_color", color);
		m_textureShader->upload("u_tilingFactor", 1.0f);
		m_whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();
		RenderCommand::drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawQuad(
	  const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		m_textureShader->upload("u_color", tintColor);
		m_textureShader->upload("u_tilingFactor", tilingFactor);
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();
		RenderCommand::drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		m_textureShader->upload("u_color", color);
		m_textureShader->upload("u_tilingFactor", 1.0f);
		m_whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f}) *
		                      glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();
		RenderCommand::drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& tintColor)
	{
		m_textureShader->upload("u_color", tintColor);
		m_textureShader->upload("u_tilingFactor", tilingFactor);
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f}) *
		                      glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();
		RenderCommand::drawIndexed(m_quadVertexArray);
	}

}  // namespace MRG::OpenGL
