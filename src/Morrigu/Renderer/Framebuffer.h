#ifndef MRG_CLASS_FRAMEBUFFER
#define MRG_CLASS_FRAMEBUFFER

#include "Core/Core.h"

#include <imgui.h>

#include <cstdint>

namespace MRG
{
	struct FramebufferSpecification
	{
		uint32_t width, height;
		// FramebufferFormat Format =
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual void destroy() = 0;

		[[nodiscard]] virtual ImTextureID getImTextureID() = 0;
		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const = 0;

		[[nodiscard]] static Ref<Framebuffer> create(const FramebufferSpecification& spec);

	protected:
		FramebufferSpecification m_specification;
		bool m_isDestroyed;
	};

}  // namespace MRG

#endif