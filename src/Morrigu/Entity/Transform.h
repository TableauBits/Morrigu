//
// Created by Mathis Lamidey on 2021-11-25
//

#ifndef MORRIGU_ENTITY_TRANSFORM_H
#define MORRIGU_ENTITY_TRANSFORM_H

#include "Utils/GLMIncludeHelper.h"

namespace MRG
{
	class Transform
	{
	public:
		glm::vec3 translation{0.f};
		glm::vec3 rotation{0.f};
		glm::vec3 scale{1.f};

		[[nodiscard]] glm::mat4 getTransform() const
		{
			const auto rotationMatrix = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4{1.f}, translation) * rotationMatrix * glm::scale(glm::mat4{1.f}, scale);
		}
	};
}  // namespace MRG

#endif
