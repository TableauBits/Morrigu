#include "Context.h"

#include "Core/Core.h"
#include "Renderer/APIs/OpenGL/Context.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Context* Context::create(GLFWwindow* window)
	{
		switch (Renderer::getAPI()) {
		case RendererAPI::OpenGL: {
			return new OpenGL::Context{window};
		} break;

		case RendererAPI::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI());
			return nullptr;
		} break;
		}
	}
}  // namespace MRG
