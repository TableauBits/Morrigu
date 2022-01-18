//
// Created by Mathis Lamidey on 2022-01-04
//

#ifndef COMP_LC_POINTLIGHT_H
#define COMP_LC_POINTLIGHT_H

#include <Morrigu.h>

namespace Components
{
	struct PointLight
	{
		glm::vec4 color{};

		// x: constant, y: linear, z: quadratic. w is ignored.
		// attenuation(d) = 1/(x + y*d + z*d*d)
		glm::vec4 attenuation{};
	};
}  // namespace Components

#endif
