#ifndef MRG_CLASS_IMGUILAYER
#define MRG_CLASS_IMGUILAYER

#include "Core/Layer.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "imgui.h"

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
		bool onMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		bool onMouseMovedEvent(MouseMovedEvent& event);
		bool onMouseScrolledEvent(MouseScrolledEvent& event);
		bool onKeyPressedEvent(KeyPressedEvent& event);
		bool onKeyReleasedEvent(KeyReleasedEvent& event);
		bool onKeyTypedEvent(KeyTypedEvent& event);
		bool onWindowResizeEvent(WindowResizeEvent& event);

		void updateSpecialKeys(ImGuiIO& io);

	private:
		float m_cumulativeTime;
	};
}  // namespace MRG

#endif