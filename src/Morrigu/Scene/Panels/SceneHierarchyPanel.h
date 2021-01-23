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
		explicit SceneHierarchyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();

		Entity selectedEntity;

	private:
		void drawEntityNode(Entity& entity);
		void drawComponents(Entity& entity);

		Ref<Scene> m_context;
	};
}  // namespace MRG

#endif
