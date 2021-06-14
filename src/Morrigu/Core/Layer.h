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

		virtual ~Layer() = 0;

		Layer& operator=(const Layer&) = delete;
		Layer& operator=(Layer&&) = delete;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep) {}
		virtual void onImGuiUpdate(Timestep) {}
		virtual void onEvent(Event&) {}

		Application* application{nullptr};
		std::vector<RenderData> renderables{};
		Camera mainCamera{};
	};
}  // namespace MRG

#endif  // MORRIGU_LAYER_H
