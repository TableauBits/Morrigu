//
// Created by Mathis Lamidey on 2021-11-22
//

#ifndef MORRIGU_UTILSMATHS_H
#define MORRIGU_UTILSMATHS_H

#include "Utils/GLMIncludeHelper.h"

#include <tuple>

namespace MRG::Utils::Maths
{
	struct TransformComponents
	{
		glm::vec3 translation{}, rotation{}, scale{};
	};
	[[nodiscard]] TransformComponents decomposeTransform(const glm::mat4& transform);
}  // namespace MRG::Utils::Maths

#endif
