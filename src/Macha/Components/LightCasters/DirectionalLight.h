//
// Created by Mathis Lamidey on 2021-11-25
//

#ifndef COMP_LC_DIRECTIONAL_H
#define COMP_LC_DIRECTIONAL_H

#include <Morrigu.h>

namespace Components
{
	struct DirectionalLight
	{
		glm::vec4 direction{};
		glm::vec4 color{};
	};
}  // namespace Components

#endif
