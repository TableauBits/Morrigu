//
// Created by Mathis Lamidey on 2021-04-03.
//

#ifndef MORRIGU_APPLICATION_H
#define MORRIGU_APPLICATION_H

#include "Core/LayerStack.h"
int main();

namespace MRG
{
	class Application
	{
	public:
		void pushLayer(Layer* newLayer);
		Layer* popLayer();

	private:
		void init();
		void run(){};
		void cleanup(){};

		friend int ::main();

		LayerStack m_layers;
	};

	[[nodiscard]] extern Application* createApplication();
}  // namespace MRG

#endif  // MORRIGU_APPLICATION_H
