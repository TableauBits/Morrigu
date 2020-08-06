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

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static auto getAPI() { return RenderingAPI::getAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 projectionViewMatrix;
		};

		static SceneData* s_sceneData;
	};
}  // namespace MRG

#endif