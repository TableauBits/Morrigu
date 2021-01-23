#include "Context.h"

#include "Renderer/APIs/OpenGL/Context.h"
#include "Renderer/APIs/Vulkan/Context.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Scope<Context> Context::create(GLFWwindow* window)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createScope<OpenGL::Context>(window);
		}

		case RenderingAPI::API::Vulkan: {
			return createScope<Vulkan::Context>(window);
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}
}  // namespace MRG
