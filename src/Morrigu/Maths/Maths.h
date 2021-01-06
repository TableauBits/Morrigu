#ifndef MRG_MATHS_FUNCTIONS
#define MRG_MATHS_FUNCTIONS

#include "Core/GLMIncludeHelper.h"

namespace MRG::Maths
{
	bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
}  // namespace MRG::Maths

#endif
