#include "RenderCommand.h"

#include "Renderer/APIs/OpenGL/RenderingAPI.h"

namespace MRG
{
	RenderingAPI* RenderCommand::s_renderingAPI = new OpenGL::RenderingAPI;
}