//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_APPLICATION_H
#define MORRIGU_APPLICATION_H

#include "Core/LayerStack.h"
#include "Rendering/VkRenderer.h"

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

		void run();

		void pushLayer(Layer* newLayer);
		Layer* popLayer();

		void setWindowTitle(const char* title) const;
		void uploadMesh(Mesh& mesh);

		void drawMesh(const Mesh& mesh, const Camera& camera);

	private:
		void init();

		void onEvent(Event& event);
		bool onClose(WindowCloseEvent& resizeEvent);
		bool onResize(WindowResizeEvent& resizeEvent);

		friend class VkRenderer;

		bool m_isRunning = true;
		float m_lastTime = 0.f;
		ApplicationSpecification m_specification;
		LayerStack m_layers;

		VkRenderer m_renderer;
	};
}  // namespace MRG

#endif  // MORRIGU_APPLICATION_H
