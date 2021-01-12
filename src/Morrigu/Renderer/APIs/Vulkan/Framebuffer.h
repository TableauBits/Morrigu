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
		~Framebuffer() override;

		void destroy() override;
		void resize(uint32_t width, uint32_t height) override;

		void invalidate();

		[[nodiscard]] ImTextureID getImTextureID() override;
		[[nodiscard]] const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] const std::array<ImVec2, 2>& getUVMapping() const override { return m_UVMapping; }

		[[nodiscard]] VkFramebuffer getHandle() { return m_handle; }
		[[nodiscard]] VkImage getColorAttachment() const { return m_colorAttachment.handle; }

	private:
		ImTextureID m_ImTextureID = nullptr;
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 0}, ImVec2{1, 1}};
		VkFramebuffer m_handle{};
		LightVulkanImage m_colorAttachment{}, m_objectIDBuffer{}, m_depthAttachment{};
		VkSampler m_sampler{};
	};

}  // namespace MRG::Vulkan

#endif
