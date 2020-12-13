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

		void invalidate();

		[[nodiscard]] ImTextureID getImTextureID() override;
		[[nodiscard]] const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] const std::array<ImVec2, 2> getUVMapping() const override { return m_UVMapping; }
		[[nodiscard]] const VkFramebuffer getHandle() { return m_handle; }
		[[nodiscard]] const VkImage getColorAttachment() { return m_colorAttachment.handle; }

	private:
		ImTextureID m_ImTextureID = nullptr;
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 0}, ImVec2{1, 1}};
		VkFramebuffer m_handle;
		LightVulkanImage m_colorAttachment, m_depthAttachment;
		VkSampler m_sampler;
	};

}  // namespace MRG::Vulkan

#endif