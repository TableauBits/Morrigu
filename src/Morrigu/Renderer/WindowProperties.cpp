#include "WindowProperties.h"

#include "Renderer/APIs/Vulkan/WindowProperties.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Scope<WindowProperties> WindowProperties::create(const char* title, uint32_t width, uint32_t height, bool vSync)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createScope<WindowProperties>(WindowProperties{title, width, height, vSync});
		} break;

		case RenderingAPI::API::Vulkan: {
			return createScope<Vulkan::WindowProperties>(title, width, height, vSync);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG