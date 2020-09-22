#ifndef MRG_CLASS_RENDERER2D
#define MRG_CLASS_RENDERER2D

#include "Renderer/OrthoCamera.h"
#include "Renderer/Textures.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace MRG
{
	class Generic2DRenderer
	{
	public:
		virtual ~Generic2DRenderer() = default;

		virtual void init(GLFWwindow* window) = 0;
		virtual void shutdown() = 0;

		virtual void beginScene(const OrthoCamera& camera) = 0;
		virtual void endScene() = 0;

		virtual void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) = 0;
		virtual void drawQuad(const glm::vec3& position,
		                      const glm::vec2& size,
		                      const Ref<Texture2D>& texture,
		                      float tilingFactor = 1.0f,
		                      const glm::vec4& tintColor = glm::vec4(1.0f)) = 0;

		virtual void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color) = 0;
		virtual void drawRotatedQuad(const glm::vec3& position,
		                             const glm::vec2& size,
		                             float rotation,
		                             const Ref<Texture2D>& texture,
		                             float tilingFactor = 1.0f,
		                             const glm::vec4& tintColor = glm::vec4(1.0f)) = 0;
	};

	class Renderer2D
	{
	public:
		static void init(GLFWwindow* window);
		static void shutdown();

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		/// Primitives

		// Quad
		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec2& position,
		                     const glm::vec2& size,
		                     const Ref<Texture2D>& texture,
		                     float tilingFactor = 1.0f,
		                     const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawQuad(const glm::vec3& position,
		                     const glm::vec2& size,
		                     const Ref<Texture2D>& texture,
		                     float tilingFactor = 1.0f,
		                     const glm::vec4& tintColor = glm::vec4(1.0f));

		static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRotatedQuad(const glm::vec2& position,
		                            const glm::vec2& size,
		                            float rotation,
		                            const Ref<Texture2D>& texture,
		                            float tilingFactor = 1.0f,
		                            const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawRotatedQuad(const glm::vec3& position,
		                            const glm::vec2& size,
		                            float rotation,
		                            const Ref<Texture2D>& texture,
		                            float tilingFactor = 1.0f,
		                            const glm::vec4& tintColor = glm::vec4(1.0f));

	private:
		static Scope<Generic2DRenderer> s_renderer;
	};
}  // namespace MRG

#endif