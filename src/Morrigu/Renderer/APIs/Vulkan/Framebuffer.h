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

		[[nodiscard]] virtual ImTextureID getImTextureID() override;
		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] const VkFramebuffer getHandle() { return m_handle; }

	private:
		ImTextureID m_ImTextureID = nullptr;

		VkFramebuffer m_handle;
		LightVulkanImage m_depthAttachment, m_colorAttachment;
		VkSampler m_sampler;
	};

}  // namespace MRG::Vulkan

#endif