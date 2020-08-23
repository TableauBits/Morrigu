#ifndef MRG_CLASS_IMGUILAYER
#define MRG_CLASS_IMGUILAYER

#include "Core/Layer.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include <imgui.h>

namespace MRG
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate([[maybe_unused]] Timestep ts) override {}
		void onEvent([[maybe_unused]] Event& event) override {}

		void begin();
		void end();

	private:
		// float m_cumulativeTime;
	};
}  // namespace MRG

#endif