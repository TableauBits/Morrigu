#include <vulkan/vulkan_core.h>

#include <stdexcept>

#define MRG_VKVALIDATE(expression, failure_message)                                                                                        \
	if (expression != VK_SUCCESS) {                                                                                                        \
		throw std::runtime_error(failure_message);                                                                                         \
	}
