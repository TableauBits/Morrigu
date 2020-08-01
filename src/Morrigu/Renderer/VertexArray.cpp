#include "VertexArray.h"

#include "Renderer/APIs/OpenGL/VertexArray.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	VertexArray* VertexArray::create()
	{
		switch (Renderer::getAPI()) {
		case RendererAPI::OpenGL: {
			return new OpenGL::VertexArray;
		} break;

		case RendererAPI::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI());
			return nullptr;
		} break;
		}
	}
}  // namespace MRG