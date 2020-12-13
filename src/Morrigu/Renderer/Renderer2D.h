#ifndef MRG_CLASS_RENDERER2D
#define MRG_CLASS_RENDERER2D

#include "Core/GLMIncludeHelper.h"
#include "Renderer/Buffers.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/OrthoCamera.h"
#include "Renderer/Textures.h"

#include <GLFW/glfw3.h>

#include <array>

namespace MRG
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float tilingFactor;

		static inline std::initializer_list<MRG::BufferElement> layout = {{MRG::ShaderDataType::Float3, "a_position"},
		                                                                  {MRG::ShaderDataType::Float4, "a_color"},
		                                                                  {MRG::ShaderDataType::Float2, "a_texCoord"},
		                                                                  {MRG::ShaderDataType::Float, "a_texIndex"},
		                                                                  {MRG::ShaderDataType::Float, "a_tilingFactor"}};
	};

	struct RenderingStatistics
	{
		uint32_t drawCalls = 0;
		uint32_t quadCount = 0;

		auto getVertexCount() const { return quadCount * 4; }
		auto getIndexCount() const { return quadCount * 6; }
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

		virtual void setRenderTarget(Ref<Framebuffer> renderTarget) = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;

		virtual void resetStats() = 0;
		virtual RenderingStatistics getStats() const = 0;

		static const uint32_t maxQuads = 10000;
		static const uint32_t maxVertices = 4 * maxQuads;
		static const uint32_t maxIndices = 6 * maxQuads;
		static const uint32_t maxTextureSlots = 32;

	protected:
		const std::size_t m_quadVertexCount = 4;
		const glm::vec4 m_quadVertexPositions[4] = {
		  {-0.5f, -0.5f, 0.0f, 1.f}, {0.5f, -0.5f, 0.0f, 1.f}, {0.5f, 0.5f, 0.0f, 1.f}, {-0.5f, 0.5f, 0.0f, 1.f}};
		const glm::vec2 m_textureCoordinates[4] = {{0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f}};

		uint32_t m_quadIndexCount = 0;
		QuadVertex* m_qvbBase = nullptr;
		QuadVertex* m_qvbPtr = nullptr;
		std::array<Ref<Texture2D>, maxTextureSlots> m_textureSlots;
		std::size_t m_textureSlotindex = 1;
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

		static void setRenderTarget(Ref<Framebuffer> renderTarget);
		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void setClearColor(const glm::vec4& color);
		static void clear();

		static void resetStats();
		static RenderingStatistics getStats();

	private:
		static GLFWwindow* s_windowHandle;
		static Scope<Generic2DRenderer> s_renderer;
	};
}  // namespace MRG

#endif