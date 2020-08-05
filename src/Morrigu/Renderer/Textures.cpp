#include "Textures.h"

#include "Renderer/APIs/OpenGL/Textures.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Ref<Texture2D> Texture2D::create(const std::string& path)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Texture2D>(path);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERING API OPTION ! ({})", RenderingAPI::getAPI());
			return nullptr;
		} break;
		}
	}
}  // namespace MRG