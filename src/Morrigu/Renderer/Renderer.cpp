#include "Renderer.h"

#include "Renderer/Renderer2D.h"

// TEMPORARY
#include "Renderer/APIs/OpenGL/Shader.h"

namespace MRG
{
	Scope<Renderer::SceneData> Renderer::s_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init()
	{
		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::shutdown() { Renderer2D::shutdown(); }

	void Renderer::onWindowResize(uint32_t width, uint32_t height) { RenderCommand::setViewport(0, 0, width, height); }
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