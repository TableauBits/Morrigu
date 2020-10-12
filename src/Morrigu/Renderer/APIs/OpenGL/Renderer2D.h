#ifndef MRG_OPENGL_IMPL_RENDERER2D
#define MRG_OPENGL_IMPL_RENDERER2D

#include "Renderer/VertexArray.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/Shader.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	class Renderer2D : public MRG::Generic2DRenderer
	{
	public:
		virtual ~Renderer2D() = default;

		void init() override;
		void shutdown() override;

		void onWindowResize(uint32_t width, uint32_t height) override;

		bool beginFrame() override;
		bool endFrame() override;

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

		void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override { glViewport(x, y, width, height); }
		void setClearColor(const glm::vec4& color) override { glClearColor(color.r, color.g, color.b, color.a); }
		void clear() override { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	private:
		void drawIndexed(const Ref<VertexArray>& vertexArray);

		Ref<MRG::VertexArray> m_quadVertexArray;
		Ref<MRG::Shader> m_textureShader;
		Ref<MRG::Texture2D> m_whiteTexture;
	};
}  // namespace MRG::OpenGL

#endif