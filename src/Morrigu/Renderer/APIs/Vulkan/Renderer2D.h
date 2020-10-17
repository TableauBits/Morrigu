#ifndef MRG_VULKAN_IMPL_RENDERER2D
#define MRG_VULKAN_IMPL_RENDERER2D

#include "Renderer/Renderer2D.h"

#include "Renderer/APIs/Vulkan/Buffers.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"

#include <vulkan/vulkan.hpp>

namespace MRG::Vulkan
{
	class Renderer2D : public Generic2DRenderer
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

		void setViewport(uint32_t, uint32_t, uint32_t, uint32_t) override {}
		void setClearColor(const glm::vec4&) override {}
		void clear() override {}

	private:
		WindowProperties* m_data;
		Ref<Shader> m_textureShader;
		uint32_t m_imageIndex;
		std::size_t m_maxFramesInFlight = 2;
		std::vector<VkSemaphore> m_imageAvailableSemaphores, m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences, m_imagesInFlight;
		Ref<VertexBuffer> m_vertexBuffer;
		Ref<IndexBuffer> m_indexBuffer;

		bool m_shouldRecreateSwapChain = false;
	};
}  // namespace MRG::Vulkan

#endif