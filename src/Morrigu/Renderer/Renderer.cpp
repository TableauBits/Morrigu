#include "Renderer.h"

// TEMPORARY
#include "Renderer/APIs/OpenGL/Shader.h"

namespace MRG
{
	Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(const OrthoCamera& camera) { s_sceneData->projectionViewMatrix = camera.getProjectionViewMatrix(); }
	void Renderer::endScene() {}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		std::static_pointer_cast<OpenGL::Shader>(shader)->uploadUniform("u_viewProjection", s_sceneData->projectionViewMatrix);
		std::static_pointer_cast<OpenGL::Shader>(shader)->uploadUniform("u_transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}  // namespace MRG