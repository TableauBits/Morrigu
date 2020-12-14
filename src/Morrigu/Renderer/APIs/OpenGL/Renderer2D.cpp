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

		m_quadVertexBuffer = VertexBuffer::create(maxVertices * sizeof(QuadVertex));
		m_quadVertexBuffer->layout = QuadVertex::layout;
		m_quadVertexArray->addVertexBuffer(m_quadVertexBuffer);

		m_qvbBase = new QuadVertex[maxVertices];
		uint32_t* quadIndices = new uint32_t[maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < maxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, maxIndices);
		m_quadVertexArray->setIndexBuffer(quadIB);
		delete[] quadIndices;

		m_whiteTexture = Texture2D::create(1, 1);
		auto whiteTextureData = 0xffffffff;
		m_whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		int32_t samplers[maxTextureSlots];
		for (uint32_t i = 0; i < maxTextureSlots; ++i) samplers[i] = i;

		m_textureShader = Shader::create("resources/shaders/texture");
		m_textureShader->bind();
		m_textureShader->upload("u_textures", samplers, maxTextureSlots);

		m_textureSlots[0] = m_whiteTexture;
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		m_textureShader->destroy();
		m_whiteTexture->destroy();
		m_quadVertexArray->destroy();

		for (const auto& texture : m_textureSlots) {
			if (texture != nullptr)
				texture->destroy();
		}

		if (m_framebuffer != nullptr) {
			m_framebuffer->destroy();
		}

		delete[] m_qvbBase;
	}

	void Renderer2D::onWindowResize(uint32_t width, uint32_t height) { setViewport(0, 0, width, height); }

	bool Renderer2D::beginFrame()
	{
		MRG_PROFILE_FUNCTION();

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

		m_textureSlotindex = 1;
		m_sceneInProgress = true;
	}

	void Renderer2D::endScene()
	{
		MRG_PROFILE_FUNCTION();

		uint32_t dataSize = static_cast<uint32_t>((uint8_t*)m_qvbPtr - (uint8_t*)m_qvbBase);
		m_quadVertexBuffer->setData(m_qvbBase, dataSize);

		flush();
		m_sceneInProgress = false;
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawQuad(
	  const glm::vec3& position, const glm::vec2& size, const Ref<MRG::Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		float texIndex = 0.f;
		for (uint32_t i = 0; i < m_textureSlotindex; ++i) {
			if (*m_textureSlots[i].get() == *texture.get()) {
				texIndex = static_cast<float>(i);
				break;
			}
		}

		if (texIndex == 0.f) {
			if (m_textureSlotindex >= maxTextureSlots)
				flushAndReset();

			texIndex = static_cast<float>(m_textureSlotindex);
			m_textureSlots[m_textureSlotindex] = texture;
			++m_textureSlotindex;
		}

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = tintColor;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f}) *
		                 glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<MRG::Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		float texIndex = 0.f;
		for (uint32_t i = 0; i < m_textureSlotindex; ++i) {
			if (*m_textureSlots[i].get() == *texture.get()) {
				texIndex = static_cast<float>(i);
				break;
			}
		}

		if (texIndex == 0.f) {
			if (m_textureSlotindex >= maxTextureSlots)
				flushAndReset();

			texIndex = static_cast<float>(m_textureSlotindex);
			m_textureSlots[m_textureSlotindex] = texture;
			++m_textureSlotindex;
		}

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f}) *
		                 glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = tintColor;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::setRenderTarget(Ref<MRG::Framebuffer> renderTarget)
	{
		if (renderTarget == nullptr) {
			resetRenderTarget();
			return;
		}

		if (m_sceneInProgress)
			flushAndReset();

		m_framebuffer = std::static_pointer_cast<Framebuffer>(renderTarget);
		m_framebuffer->bind();
	}

	void Renderer2D::resetRenderTarget()
	{
		if (m_sceneInProgress)
			flushAndReset();

		m_framebuffer->unbind();
		m_framebuffer = nullptr;
	}

	void Renderer2D::resetStats() { m_stats = {}; }

	RenderingStatistics Renderer2D::getStats() const { return m_stats; }

	void Renderer2D::flush()
	{
		if (m_quadIndexCount == 0)
			return;

		for (uint32_t i = 0; i < m_textureSlotindex; ++i) m_textureSlots[i]->bind(i);

		drawIndexed(m_quadVertexArray, m_quadIndexCount);
		++m_stats.drawCalls;
	}

	void Renderer2D::flushAndReset()
	{
		endScene();

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;
		m_textureSlotindex = 1;
	}

	void Renderer2D::drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		MRG_PROFILE_FUNCTION();

		const auto indexCount = count ? count : vertexArray->getIndexBuffer()->getCount();

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}  // namespace MRG::OpenGL
