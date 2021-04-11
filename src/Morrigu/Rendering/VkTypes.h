//
// Created by mathi on 2021-04-11.
//

#ifndef MORRIGU_VKTYPES_H
#define MORRIGU_VKTYPES_H

#include "Core/Core.h"

#include <vulkan/vulkan.h>

#include <exception>

#define MRG_VK_CHECK(expression, error_message)                                                                                            \
	{                                                                                                                                      \
		if ((expression) != VK_SUCCESS) {                                                                                                  \
			throw std::runtime_error(failure_message);                                                                                     \
		}                                                                                                                                  \
	}

#endif  // MORRIGU_VKTYPES_H
