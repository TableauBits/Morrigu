#include "RenderingAPI.h"

#include "Renderer/APIs/OpenGL/RenderingAPI.h"
#include "Renderer/APIs/Vulkan/RenderingAPI.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	RenderingAPI::API RenderingAPI::s_API = RenderingAPI::API::Vulkan;

	Scope<RenderingAPI> RenderingAPI::create()
	{
		switch (s_API) {
		case RenderingAPI::API::OpenGL: {
			return createScope<OpenGL::RenderingAPI>();
		} break;

		case RenderingAPI::API::Vulkan: {
			return createScope<Vulkan::RenderingAPI>();
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG