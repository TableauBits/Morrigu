#ifndef MRG_CLASS_SCENEHIERARCHYPANEL
#define MRG_CLASS_SCENEHIERARCHYPANEL

#include "Core/Core.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace MRG
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();

	private:
		void drawEntityNode(Entity& entity);

		Ref<Scene> m_context;
		Entity m_selectedEntity;
	};
}  // namespace MRG

#endif
