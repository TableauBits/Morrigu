#ifndef MRG_CLASS_SCENESERIALIZER
#define MRG_CLASS_SCENESERIALIZER

#include "Core/Core.h"
#include "Scene/Scene.h"

namespace MRG
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void serialize(const std::string& filepath);
		bool deserialize(const std::string& filepath);

	private:
		Ref<Scene> m_scene;
	};
}  // namespace MRG

#endif
