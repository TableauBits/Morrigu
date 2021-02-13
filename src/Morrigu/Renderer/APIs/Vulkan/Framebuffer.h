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
		explicit Framebuffer(const FramebufferSpecification& spec);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = delete;
		~Framebuffer() override;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = delete;

		void destroy() override;
		void resize(uint32_t width, uint32_t height) override;

		void invalidate();

		[[nodiscard]] ImTextureID getImTextureID(uint32_t index) override;
		[[nodiscard]] const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] const std::array<ImVec2, 2>& getUVMapping() const override { return m_UVMapping; }

		[[nodiscard]] auto getHandle() const { return m_handle; }
		[[nodiscard]] auto getColorAttachment(std::size_t index) const
		{
			MRG_CORE_ASSERT(index < m_colorAttachments.size(), "invalid index!")
			return m_colorAttachments[index];
		}

	private:
		VkFramebuffer m_handle{};
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 0}, ImVec2{1, 1}};

		std::vector<ImTextureID> m_ImTextureIDs{};
		std::vector<LightVulkanImage> m_colorAttachments{};
		LightVulkanImage m_depthAttachment{};
		VkSampler m_sampler{};

		VkPipeline m_pipeline{};
		VkRenderPass m_clearingRP{}, m_mainRP{};
	};

}  // namespace MRG::Vulkan

#endif
