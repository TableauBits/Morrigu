#include "Textures.h"

#include "Renderer/APIs/OpenGL/Textures.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Texture2D>(width, height);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}

	Ref<Texture2D> Texture2D::create(const std::string& path)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Texture2D>(path);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG