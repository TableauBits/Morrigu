#include "Shader.h"

#include "Renderer/APIs/OpenGL/Shader.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Ref<Shader> Shader::create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Shader>(vertexSrc, fragmentSrc);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERING API OPTION ! ({})", RenderingAPI::getAPI());
			return nullptr;
		} break;
		}
	}
}  // namespace MRG