//
// Created by Mathis Lamidey on 2021-08-29.
//

#include "UtilityLayers.h"

namespace MRG
{
	Ref<UI::Font> StandardLayer::createFont(const std::string& filePath) { return application->renderer->createFont(filePath); }
	Ref<UI::Text> StandardLayer::createText(const std::string& content, const Ref<UI::Font>& font, const glm::vec2& position)
	{
		return createRef<UI::Text>(content, font, position, application->renderer.get(), registry);
	}
	Ref<Shader> StandardLayer::createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	{
		return application->renderer->createShader(vertexShaderFile.c_str(), fragmentShaderFile.c_str());
	}
	Ref<Entity> StandardLayer::createEntity() { return application->renderer->createEntity(registry); }
	Ref<Framebuffer> StandardLayer::createFramebuffer(const FramebufferSpecification& spec)
	{
		return application->renderer->createFrameBuffer(spec);
	}

	UILayer::UILayer() : mainCamera{PixelPerfectCamera{}} {}
	void UILayer::onAttach()
	{
		mainCamera.width  = static_cast<float>(application->renderer->spec.windowWidth);
		mainCamera.height = static_cast<float>(application->renderer->spec.windowHeight);
		mainCamera.recalculateViewProjection();
	}
	void UILayer::onEvent(MRG::Event& event)
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>(
		  [this](MRG::WindowResizeEvent& resizeEvent) { return mainCamera.onResize(resizeEvent); });
	}
}  // namespace MRG
