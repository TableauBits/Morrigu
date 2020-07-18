#ifndef MRG_CLASS_IMGUILAYER
#define MRG_CLASS_IMGUILAYER

#include "Core/Layer.h"

namespace MRG
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& event) override;

	private:
		float m_cumulativeTime;
	};
}  // namespace MRG

#endif