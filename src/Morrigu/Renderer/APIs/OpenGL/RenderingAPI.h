#ifndef MRG_OPENGL_IMPL_RENDRINGAPI
#define MRG_OPENGL_IMPL_RENDRINGAPI

#include "Renderer/RenderingAPI.h"

namespace MRG::OpenGL
{
	class RenderingAPI : public MRG::RenderingAPI
	{
	public:
		void init() override;

		void setClearColor(const glm::vec4& color) override;
		void clear() override;

		void drawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}  // namespace MRG::OpenGL

#endif