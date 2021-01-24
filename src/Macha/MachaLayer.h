#include "Renderer/EditorCamera.h"
#include "Scene/Panels/SceneHierarchyPanel.h"

#include <Morrigu.h>

namespace MRG
{
	class MachaLayer : public Layer
	{
	public:
		MachaLayer();
		MachaLayer(const MachaLayer&) = delete;
		MachaLayer(MachaLayer&&) = delete;
		~MachaLayer() override = default;

		MachaLayer& operator=(const MachaLayer&) = delete;
		MachaLayer& operator=(MachaLayer&&) = delete;

		void onAttach() override;
		void onDetach() override;

		void onUpdate(Timestep ts) override;
		void onImGuiRender() override;
		void onEvent(Event& event) override;

	private:
		bool onKeyPressed(KeyPressedEvent& e);
		bool onMousePressed(MouseButtonPressedEvent& e);

		void newScene();
		void openScene();
		void saveScene();

		Ref<Framebuffer> m_renderTarget;

		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = {0.f, 0.f};
		ImVec2 m_viewportWindowPosition = {0.f, 0.f};
		ImVec2 m_viewportPosition = {0.f, 0.f};

		Ref<Scene> m_activeScene;
		int m_gizmoType = -1;

		Timestep m_frameTime;

		EditorCamera m_editorCamera;

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;
	};
}  // namespace MRG
