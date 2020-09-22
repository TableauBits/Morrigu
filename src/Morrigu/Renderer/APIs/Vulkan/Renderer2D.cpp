#include "Renderer2D.h"

namespace MRG::Vulkan
{
	void Renderer2D::init(GLFWwindow* window) { m_data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window)); }

	void Renderer2D::shutdown() {}

	void Renderer2D::beginScene(const OrthoCamera&) {}

	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const glm::vec4&) {}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const Ref<Texture2D>&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const Ref<Texture2D>&, float, const glm::vec4&) {}
}  // namespace MRG::Vulkan
