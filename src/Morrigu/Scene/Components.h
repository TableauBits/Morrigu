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

		ScriptableEntity* (*instanciateScript)();
		void (*destroyScript)(NativeScriptComponent*);

		template<typename T>
		void bind()
		{
			instanciateScript = +[]() { return static_cast<ScriptableEntity*>(new T{}); };
			destroyScript = +[](NativeScriptComponent* nsc) {
				delete nsc->instance;
				nsc->instance = nullptr;
			};
		}
	};

}  // namespace MRG

#endif
