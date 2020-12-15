#ifndef MRG_VULKAN_IMPL_FRAMEBUFFER
#define MRG_VULKAN_IMPL_FRAMEBUFFER

#include "Renderer/APIs/Vulkan/Textures.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"
#include "Renderer/Framebuffer.h"

namespace MRG::Vulkan
{
	class Framebuffer : public MRG::Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		virtual ~Framebuffer();

		void destroy() override;

		void invalidate() override;

		[[nodiscard]] ImTextureID getImTextureID() override;
		[[nodiscard]] const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] const std::array<ImVec2, 2>& getUVMapping() const override { return m_UVMapping; }

		[[nodiscard]] VkFramebuffer getHandle() { return m_handle; }
		[[nodiscard]] VkImage getColorAttachment() { return m_colorAttachment.handle; }

		void updateView(VkCommandBuffer commandBuffer, bool isClearCommand = false);
		VkExtent2D getFramebufferDimensions() { return {m_FBWidth, m_FBHeight}; }

	private:
		ImTextureID m_ImTextureID = nullptr;
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 0}, ImVec2{1, 1}};
		VkFramebuffer m_handle;
		LightVulkanImage m_colorAttachment, m_depthAttachment;
		Texture2D m_renderTexture;
		VkSampler m_sampler;
		uint32_t m_FBWidth, m_FBHeight;
	};

}  // namespace MRG::Vulkan

#endif