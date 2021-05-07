//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_VKTYPES_H
#define MORRIGU_VKTYPES_H

#include "Core/Core.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <exception>

// This macro should not be necessary, because Vulkan HPP throws by default, and checks the C API results on its own.
#define MRG_VK_CHECK(expression, error_message)                                                                                            \
	{                                                                                                                                      \
		if ((expression) != vk::Result::eSuccess) { throw std::runtime_error(error_message); }                                             \
	}

#endif  // MORRIGU_VKTYPES_H
