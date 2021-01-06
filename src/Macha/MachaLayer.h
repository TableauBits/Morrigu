#include "Scene/Panels/SceneHierarchyPanel.h"
#include <Morrigu.h>

namespace MRG
{
	class MachaLayer : public Layer
	{
	public:
		MachaLayer();
		virtual ~MachaLayer() = default;

		void onAttach() override;
		void onDetach() override;

		void onUpdate(Timestep ts) override;
		void onImGuiRender() override;
		void onEvent(Event& event) override;

	private:
		bool onKeyPressed(KeyPressedEvent& e);

		void newScene();
		void openScene();
		void saveScene();

		Ref<Framebuffer> m_renderTarget;

		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = {0.f, 0.f};

		Ref<Scene> m_activeScene;
		int m_gizmoType = -1;

		Timestep m_frameTime;

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;
	};
}  // namespace MRG
