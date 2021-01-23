#include "Framebuffer.h"

#include "Renderer/APIs/OpenGL/Framebuffer.h"
#include "Renderer/APIs/Vulkan/Framebuffer.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Framebuffer>(spec);
		}

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::Framebuffer>(spec);
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}
}  // namespace MRG
