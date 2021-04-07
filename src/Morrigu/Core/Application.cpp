//
// Created by Mathis Lamidey on 2021-04-03.
//

#include "Application.h"

namespace MRG
{
	void Application::init() {}

	void Application::pushLayer(Layer* newLayer) { m_layers.pushLayer(newLayer); }

	Layer* Application::popLayer() { return m_layers.popLayer(); }
}  // namespace MRG
