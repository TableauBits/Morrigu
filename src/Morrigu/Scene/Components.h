#ifndef MRG_CLASSES_COMPONENTS
#define MRG_CLASSES_COMPONENTS

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

}  // namespace MRG

#endif
