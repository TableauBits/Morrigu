#include "RenderCommand.h"

#include "Renderer/APIs/OpenGL/RenderingAPI.h"

namespace MRG
{
	Scope<RenderingAPI> RenderCommand::s_renderingAPI = RenderingAPI::create();
}