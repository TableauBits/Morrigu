//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_LAYER_H
#define MORRIGU_LAYER_H

#include "Core/Timestep.h"
#include "Events/Event.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderObject.h"

namespace MRG
{
	class Application;

	class Layer
	{
	public:
		Layer()             = default;
		Layer(const Layer&) = delete;
		Layer(Layer&&)      = delete;

		virtual ~Layer() = default;

		Layer& operator=(const Layer&) = delete;
		Layer& operator=(Layer&&) = delete;

		virtual void onAttach()         = 0;
		virtual void onDetach()         = 0;
		virtual void onUpdate(Timestep) = 0;
		virtual void onEvent(Event&)    = 0;

		Application* application{nullptr};
		std::vector<RenderData> renderables{};
		Camera mainCamera{};
	};
}  // namespace MRG

#endif  // MORRIGU_LAYER_H
