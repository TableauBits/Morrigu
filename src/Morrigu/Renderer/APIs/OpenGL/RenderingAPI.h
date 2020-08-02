#ifndef MRG_OPENGL_IMPL_RENDRINGAPI
#define MRG_OPENGL_IMPL_RENDRINGAPI

#include "Renderer/RenderingAPI.h"

namespace MRG::OpenGL
{
	class RenderingAPI : public MRG::RenderingAPI
	{
	public:
		void setClearColor(const glm::vec4& color) override;
		void clear() override;

		void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}  // namespace MRG::OpenGL

#endif