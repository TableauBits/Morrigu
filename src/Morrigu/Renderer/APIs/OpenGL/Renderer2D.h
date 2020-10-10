#ifndef MRG_OPENGL_IMPL_RENDERER2D
#define MRG_OPENGL_IMPL_RENDERER2D

#include "Renderer/VertexArray.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/Shader.h"

namespace MRG::OpenGL
{
	class Renderer2D : public MRG::Generic2DRenderer
	{
	public:
		virtual ~Renderer2D() = default;

		void init() override;
		void shutdown() override;

		void beginFrame() override;
		void endFrame() override;

		void beginScene(const OrthoCamera& camera) override;
		void endScene() override;

		void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) override;
		void drawQuad(const glm::vec3& position,
		              const glm::vec2& size,
		              const Ref<Texture2D>& texture,
		              float tilingFactor = 1.0f,
		              const glm::vec4& tintColor = glm::vec4(1.0f)) override;

		void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color) override;
		void drawRotatedQuad(const glm::vec3& position,
		                     const glm::vec2& size,
		                     float rotation,
		                     const Ref<Texture2D>& texture,
		                     float tilingFactor = 1.0f,
		                     const glm::vec4& tintColor = glm::vec4(1.0f)) override;

	private:
		Ref<MRG::VertexArray> m_quadVertexArray;
		Ref<MRG::Shader> m_textureShader;
		Ref<MRG::Texture2D> m_whiteTexture;
	};
}  // namespace MRG::OpenGL

#endif