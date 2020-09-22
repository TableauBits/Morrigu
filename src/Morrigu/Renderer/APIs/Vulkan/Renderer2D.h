#ifndef MRG_VULKAN_IMPL_RENDERER2D
#define MRG_VULKAN_IMPL_RENDERER2D

#include "Renderer/Renderer2D.h"

#include "Renderer/APIs/Vulkan/WindowProperties.h"

namespace MRG::Vulkan
{
	class Renderer2D : public Generic2DRenderer
	{
	public:
		virtual ~Renderer2D() = default;

		void init(GLFWwindow* window) override;
		void shutdown() override;

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
		WindowProperties* m_data;
	};
}  // namespace MRG::Vulkan

#endif