#include "RenderingAPI.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	void RenderingAPI::setClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	void RenderingAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	void RenderingAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}
}  // namespace MRG::OpenGL