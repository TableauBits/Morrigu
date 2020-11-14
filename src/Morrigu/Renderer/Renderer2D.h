#ifndef MRG_CLASS_RENDERER2D
#define MRG_CLASS_RENDERER2D

#include "Core/GLMIncludeHelper.h"
#include "Renderer/OrthoCamera.h"
#include "Renderer/Textures.h"

#include <GLFW/glfw3.h>

namespace MRG
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
	};

	class Generic2DRenderer
	{
	public:
		virtual ~Generic2DRenderer() = default;

		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual void onWindowResize(uint32_t width, uint32_t height) = 0;

		virtual bool beginFrame() = 0;
		virtual bool endFrame() = 0;

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

		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
	};

	class Renderer2D
	{
	public:
		static void init(GLFWwindow* window);
		static void shutdown();

		static void onWindowResize(uint32_t width, uint32_t height);

		static bool beginFrame();
		static bool endFrame();

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		[[nodiscard]] static GLFWwindow* getGLFWWindow() { return s_windowHandle; }

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

		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void setClearColor(const glm::vec4& color);

	private:
		static GLFWwindow* s_windowHandle;
		static Scope<Generic2DRenderer> s_renderer;
	};
}  // namespace MRG

#endif