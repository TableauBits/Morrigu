#ifndef MRG_CLASS_RENDERER2D
#define MRG_CLASS_RENDERER2D

#include "OrthoCamera.h"

#include <glm/glm.hpp>

namespace MRG
{
	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthoCamera& camera);
		static void endScene();

		/// Primitives

		// Quad
		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	};
}  // namespace MRG

#endif