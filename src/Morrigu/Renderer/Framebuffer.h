#ifndef MRG_CLASS_FRAMEBUFFER
#define MRG_CLASS_FRAMEBUFFER

#include "Core/Core.h"

#include <imgui.h>

#include <array>
#include <cstdint>

namespace MRG
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RGBA16,

		// Depth
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	[[nodiscard]] bool isDepthFormat(MRG::FramebufferTextureFormat format);

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format) : textureFormat(format) {}  // NOLINT (explicit)

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> newAttachments)
		    : attachments(newAttachments)
		{}

		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t width = 0, height = 0;
		FramebufferAttachmentSpecification attachments;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = delete;
		virtual ~Framebuffer() = default;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = delete;

		virtual void destroy() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		[[nodiscard]] virtual ImTextureID getImTextureID(uint32_t index = 0) = 0;
		[[nodiscard]] virtual const std::array<ImVec2, 2>& getUVMapping() const = 0;

		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const = 0;

		[[nodiscard]] static Ref<Framebuffer> create(const FramebufferSpecification& spec);

	protected:
		Framebuffer() = default;

		FramebufferSpecification m_specification;
		std::vector<FramebufferTextureSpecification> m_colorAttachmentsSpecifications{};
		FramebufferTextureSpecification m_depthAttachmentsSpecification = FramebufferTextureFormat::None;

		bool m_isDestroyed = false;
	};

}  // namespace MRG

#endif
