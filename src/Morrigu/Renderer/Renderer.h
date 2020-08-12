#ifndef MRG_CLASS_RENDERER
#define MRG_CLASS_RENDERER

#include "RenderCommand.h"

#include "Renderer/OrthoCamera.h"
#include "Shader.h"

namespace MRG
{
	class Renderer
	{
	public:
		static void init();

		static void onWindowResize(uint32_t width, uint32_t height);
		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		[[nodiscard]] inline static auto getAPI() { return RenderingAPI::getAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 projectionViewMatrix;
		};

		static Scope<SceneData> s_sceneData;
	};
}  // namespace MRG

#endif