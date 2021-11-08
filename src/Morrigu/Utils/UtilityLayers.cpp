//
// Created by Mathis Lamidey on 2021-08-29.
//

#include "UtilityLayers.h"

namespace MRG
{
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
