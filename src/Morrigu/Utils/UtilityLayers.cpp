//
// Created by Mathis Lamidey on 2021-08-29.
//

#include "UtilityLayers.h"

namespace MRG
{
	Ref<Framebuffer> StandardLayer::createFramebuffer(const FramebufferSpecification& spec)
	{
		return application->renderer->createFrameBuffer(spec);
	}
	Ref<Texture> StandardLayer::createTexture(const std::string& filePath)
	{
		return application->renderer->createTexture(filePath.c_str());
	}
	Ref<Shader> StandardLayer::createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	{
		return application->renderer->createShader(vertexShaderFile.c_str(), fragmentShaderFile.c_str());
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
