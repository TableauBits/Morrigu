#ifndef MRG_CLASS_FRAMEBUFFER
#define MRG_CLASS_FRAMEBUFFER

#include "Core/Core.h"

#include <imgui.h>

#include <array>
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
		virtual ~Framebuffer() = default;

		virtual void destroy() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		[[nodiscard]] virtual ImTextureID getImTextureID() = 0;
		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const = 0;

		[[nodiscard]] virtual const std::array<ImVec2, 2>& getUVMapping() const = 0;

		[[nodiscard]] static Ref<Framebuffer> create(const FramebufferSpecification& spec);

	protected:
		FramebufferSpecification m_specification;
		bool m_isDestroyed = false;
	};

}  // namespace MRG

#endif
