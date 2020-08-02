#include "Renderer.h"

namespace MRG
{
	Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(const OrthoCamera& camera) { s_sceneData->projectionViewMatrix = camera.getProjectionViewMatrix(); }
	void Renderer::endScene() {}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->bind();
		shader->uploadUniform("u_viewProjection", s_sceneData->projectionViewMatrix);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}  // namespace MRG