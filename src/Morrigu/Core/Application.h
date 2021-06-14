//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_APPLICATION_H
#define MORRIGU_APPLICATION_H

#include "Core/LayerStack.h"
#include "Rendering/Renderer.h"

#include "Events/ApplicationEvent.h"

int main();

namespace MRG
{
	struct ApplicationSpecification
	{
		std::string windowName;
		RendererSpecification rendererSpecification;
	};

	class Application
	{
	public:
		explicit Application(ApplicationSpecification spec);
		~Application();

		Application(const Application&) = delete;
		Application(Application&&)      = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void run();

		void pushLayer(Layer* newLayer);
		Layer* popLayer();

		void setWindowTitle(const char* title) const;

		Renderer renderer;
		float elapsedTime{};

	private:
		void init();

		void onEvent(Event& event);
		bool onClose(WindowCloseEvent& resizeEvent);
		bool onResize(WindowResizeEvent& resizeEvent);

		bool m_isRunning = true;
		float m_lastTime = 0.f;
		ApplicationSpecification m_specification;
		LayerStack m_layers;
	};
}  // namespace MRG

#endif  // MORRIGU_APPLICATION_H
