//
// Created by Mathis Lamidey on 2021-08-29.
//

#include "UtilityLayers.h"

namespace MRG
{
	StandardLayer::StandardLayer() : mainCamera{createRef<StandardCamera>()} {}
	Ref<Camera> StandardLayer::getMainCamera() { return mainCamera; }

	UILayer::UILayer() : mainCamera{createRef<PixelPerfectCamera>()} {}
	Ref<Camera> UILayer::getMainCamera() { return mainCamera; }

	void UILayer::onAttach()
	{
		mainCamera->width  = static_cast<float>(application->renderer.spec.windowWidth);
		mainCamera->height = static_cast<float>(application->renderer.spec.windowHeight);
		mainCamera->recalculateViewProjection();
	}
	void UILayer::onEvent(MRG::Event& event)
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>(
		  [this](MRG::WindowResizeEvent& resizeEvent) { return mainCamera->onResize(resizeEvent); });
	}
}  // namespace MRG
