#include "Renderer.h"

#include "Debug/Instrumentor.h"
#include "Renderer/Renderer2D.h"

namespace MRG
{
	Scope<Renderer::SceneData> Renderer::s_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init()
	{
		MRG_PROFILE_FUNCTION();

		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::shutdown() { Renderer2D::shutdown(); }

	void Renderer::onWindowResize(uint32_t width, uint32_t height) { RenderCommand::setViewport(0, 0, width, height); }
	void Renderer::beginScene(const OrthoCamera& camera) { s_sceneData->projectionViewMatrix = camera.getProjectionViewMatrix(); }
	void Renderer::endScene() {}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		MRG_PROFILE_FUNCTION();

		shader->bind();
		shader->upload("u_viewProjection", s_sceneData->projectionViewMatrix);
		shader->upload("u_transform", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}  // namespace MRG