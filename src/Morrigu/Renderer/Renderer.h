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
		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

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