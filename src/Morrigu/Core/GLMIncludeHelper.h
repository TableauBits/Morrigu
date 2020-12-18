#ifndef MRG_HELPER_GLM
#define MRG_HELPER_GLM

#include "Core/Warnings.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

DISABLE_WARNING_PUSH
DISABLE_WARNING_CONSTEXPR_IF
#include <glm/gtc/matrix_transform.hpp>
DISABLE_WARNING_POP

DISABLE_WARNING_PUSH
DISABLE_WARNING_NONSTANDARD_EXTENSION
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNING_POP

#endif