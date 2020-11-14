#include "Renderer2D.h"

#include "Core/GLMIncludeHelper.h"
#include "Debug/Instrumentor.h"
#include "Renderer/RenderingAPI.h"

namespace MRG::OpenGL
{
	void Renderer2D::init()
	{
		MRG_PROFILE_FUNCTION();

		m_quadVertexArray = VertexArray::create();

		m_quadVertexBuffer = VertexBuffer::create(m_maxVertices * sizeof(QuadVertex));
		// clang-format off
        m_quadVertexBuffer->layout = {
            { ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float4, "a_color" },
			{ ShaderDataType::Float2, "a_texCoord" }
        };
		// clang-format on
		m_quadVertexArray->addVertexBuffer(m_quadVertexBuffer);

		m_qvbBase = new QuadVertex[m_maxVertices];
		uint32_t* quadIndices = new uint32_t[m_maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_maxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, m_maxIndices);
		m_quadVertexArray->setIndexBuffer(quadIB);
		delete[] quadIndices;

		m_whiteTexture = Texture2D::create(1, 1);
		auto whiteTextureData = 0xffffffff;
		m_whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		m_textureShader = Shader::create("resources/shaders/texture");
		m_textureShader->bind();
		m_textureShader->upload("u_texture", 0);
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->destroy();
		m_whiteTexture->destroy();
		m_quadVertexArray->destroy();
	}

	void Renderer2D::onWindowResize(uint32_t width, uint32_t height) { setViewport(0, 0, width, height); }

	bool Renderer2D::beginFrame()
	{
		MRG_PROFILE_FUNCTION();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}

	bool Renderer2D::endFrame()
	{
		MRG_PROFILE_FUNCTION();

		return true;
	}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->bind();
		m_textureShader->upload("u_viewProjection", camera.getProjectionViewMatrix());

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;
	}

	void Renderer2D::endScene()
	{
		MRG_PROFILE_FUNCTION();

		uint32_t dataSize = static_cast<uint32_t>((uint8_t*)m_qvbPtr - (uint8_t*)m_qvbBase);
		m_quadVertexBuffer->setData(m_qvbBase, dataSize);

		drawIndexed(m_quadVertexArray, m_quadIndexCount);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		m_qvbPtr->position = position;
		m_qvbPtr->color = color;
		m_qvbPtr->texCoord = {0.f, 0.f};
		++m_qvbPtr;

		m_qvbPtr->position = {position.x + size.x, position.y, 0.f};
		m_qvbPtr->color = color;
		m_qvbPtr->texCoord = {1.f, 0.f};
		++m_qvbPtr;

		m_qvbPtr->position = {position.x + size.x, position.y + size.y, 0.f};
		m_qvbPtr->color = color;
		m_qvbPtr->texCoord = {1.f, 1.f};
		++m_qvbPtr;

		m_qvbPtr->position = {position.x, position.y + size.y, 0.f};
		m_qvbPtr->color = color;
		m_qvbPtr->texCoord = {0.f, 1.f};
		++m_qvbPtr;

		m_quadIndexCount += 6;
	}

	void Renderer2D::drawQuad(
	  const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->upload("u_color", tintColor);
		m_textureShader->upload("u_tilingFactor", tilingFactor);
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();

		drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->upload("u_color", color);
		m_textureShader->upload("u_tilingFactor", 1.0f);
		m_whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f}) *
		                      glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();

		drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->upload("u_color", tintColor);
		m_textureShader->upload("u_tilingFactor", tilingFactor);
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4{1.f}, position) * glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f}) *
		                      glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
		m_textureShader->upload("u_transform", transform);

		m_quadVertexArray->bind();

		drawIndexed(m_quadVertexArray);
	}

	void Renderer2D::drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		MRG_PROFILE_FUNCTION();

		const auto indexCount = count ? count : vertexArray->getIndexBuffer()->getCount();

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}  // namespace MRG::OpenGL
