#include "VertexArray.h"

#include "Renderer/APIs/OpenGL/VertexArray.h"
#include "Renderer/APIs/Vulkan/VertexArray.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Ref<VertexArray> VertexArray::create()
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::VertexArray>();
		}

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::VertexArray>();
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}
}  // namespace MRG
