#include "Renderer2D.h"

#include "Debug/Instrumentor.h"
#include "Renderer/APIs/OpenGL/Renderer2D.h"
#include "Renderer/APIs/Vulkan/Renderer2D.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	GLFWwindow* Renderer2D::s_windowHandle;
	Scope<Generic2DRenderer> Renderer2D::s_renderer;

	void Renderer2D::init(GLFWwindow* window)
	{
		MRG_PROFILE_FUNCTION();

		s_windowHandle = window;

		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			s_renderer = createScope<OpenGL::Renderer2D>();
		} break;

		case RenderingAPI::API::Vulkan: {
			s_renderer = createScope<Vulkan::Renderer2D>();
		} break;

		default:
			break;
		}

		s_renderer->init();
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->shutdown();
	}

	void Renderer2D::beginFrame()
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->beginFrame();
	}

	void Renderer2D::endFrame()
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->endFrame();
	}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->beginScene(camera);
	}

	void Renderer2D::endScene()
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->endScene();
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(glm::vec3(position, 1.0f), size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->drawQuad(position, size, color);
	}

	void Renderer2D::drawQuad(
	  const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad(glm::vec3(position, 1.0f), size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(
	  const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->drawQuad(position, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad(glm::vec3(position, 1.0f), size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->drawRotatedQuad(position, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& tintColor)
	{
		drawRotatedQuad(glm::vec3(position, 1.0f), size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& tintColor)
	{
		MRG_PROFILE_FUNCTION();

		s_renderer->drawRotatedQuad(position, size, rotation, texture, tilingFactor, tintColor);
	}
}  // namespace MRG