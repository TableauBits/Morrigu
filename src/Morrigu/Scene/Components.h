#ifndef MRG_CLASSES_COMPONENTS
#define MRG_CLASSES_COMPONENTS

#include <utility>

#include "Core/GLMIncludeHelper.h"
#include "Scene/SceneCamera.h"
#include "Scene/ScriptableEntity.h"

namespace MRG
{
	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		explicit TagComponent(std::string newTag) : tag(std::move(newTag)) {}
	};

	struct TransformComponent
	{
		glm::vec3 translation{0.f, 0.f, 0.f};
		glm::vec3 rotation{0.f, 0.f, 0.f};
		glm::vec3 scale{1.f, 1.f, 1.f};

		TransformComponent() = default;
		explicit TransformComponent(const glm::vec3& newTranslation) : translation(newTranslation) {}

		[[nodiscard]] glm::mat4 getTransform() const
		{
			return glm::translate(glm::mat4{1.f}, translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4{1.f}, scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color{1.f};

		SpriteRendererComponent() = default;
		explicit SpriteRendererComponent(const glm::vec4& newColor) : color(newColor) {}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*instanciateScript)(){};
		void (*destroyScript)(NativeScriptComponent*){};

		template<typename T>
		void bind()
		{
			instanciateScript = +[]() { return static_cast<ScriptableEntity*>(new T{}); };
			destroyScript = +[](NativeScriptComponent* nsc) {
				delete static_cast<T*>(nsc->instance);
				nsc->instance = nullptr;
			};
		}
	};

}  // namespace MRG

#endif
