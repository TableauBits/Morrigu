//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_MATERIAL_H
#define MORRIGU_MATERIAL_H

#include "Rendering/VkTypes.h"

namespace MRG
{
	struct Material
	{
		vk::Pipeline pipeline;
		vk::PipelineLayout pipelineLayout;
	};
}  // namespace MRG

#endif  // MORRIGU_MATERIAL_H
