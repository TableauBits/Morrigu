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
		TagComponent(const std::string& newTag) : tag(newTag) {}
	};

	struct TransformComponent
	{
		glm::vec3 translation{0.f, 0.f, 0.f};
		glm::vec3 rotation{0.f, 0.f, 0.f};
		glm::vec3 scale{1.f, 1.f, 1.f};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& newTranslation) : translation(newTranslation) {}

		glm::mat4 getTransform() const
		{
			const auto rotationTotal = glm::rotate(glm::mat4{1.f}, rotation.x, {1.f, 0.f, 0.f}) *
			                           glm::rotate(glm::mat4{1.f}, rotation.y, {0.f, 1.f, 0.f}) *
			                           glm::rotate(glm::mat4{1.f}, rotation.z, {0.f, 0.f, 1.f});

			return glm::translate(glm::mat4{1.f}, translation) * rotationTotal * glm::scale(glm::mat4{1.f}, scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color{1.f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& newColor) : color(newColor) {}
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
