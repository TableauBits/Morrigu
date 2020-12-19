#ifndef MRG_CLASSES_COMPONENTS
#define MRG_CLASSES_COMPONENTS

#include "Scene/SceneCamera.h"
#include "Scene/ScriptableEntity.h"

#include <glm/glm.hpp>

namespace MRG
{
	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : tag(tag) {}
	};

	struct TransformComponent
	{
		glm::mat4 transform{1.f};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : transform(transform) {}

		operator glm::mat4() { return transform; }
		operator const glm::mat4 &() const { return transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color{1.f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* instance = nullptr;

		std::function<void()> instantiateFunction;
		std::function<void()> destroyFunction;

		std::function<void(ScriptableEntity*)> onCreateFunction;
		std::function<void(ScriptableEntity*)> onDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> onUpdateFunction;

		template<typename T>
		void bind()
		{
			instantiateFunction = [&] { instance = new T(); };
			destroyFunction = [&] {
				delete static_cast<T*>(instance);
				instance = nullptr;
			};

			onCreateFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->onCreate(); };
			onDestroyFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->onDestroy(); };
			onUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { static_cast<T*>(instance)->onUpdate(ts); };
		}
	};

}  // namespace MRG

#endif
