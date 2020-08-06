#include "Context.h"

#include "Core/Core.h"
#include "Renderer/APIs/OpenGL/Context.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Scope<Context> Context::create(GLFWwindow* window)
	{
		switch (Renderer::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createScope<OpenGL::Context>(window);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG
