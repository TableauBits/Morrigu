//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_VKTYPES_H
#define MORRIGU_VKTYPES_H

#include "Core/Core.h"
#include "Utils/VMAIncludeHelper.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <exception>
#include <ranges>

struct AllocatedBuffer
{
	vk::Buffer buffer{};
	VmaAllocation allocation{};
};

struct AllocatedImage
{
	vk::Image image{};
	VmaAllocation allocation{};
};

#define MRG_VK_CHECK_HPP(expression, error_message)                                                                                        \
	{                                                                                                                                      \
		if ((expression) != vk::Result::eSuccess) { throw std::runtime_error(error_message); }                                             \
	}
// This macro is here is case we need to deal with C style VK results (for VMA initialisation, for example)
#define MRG_VK_CHECK(expression, error_message)                                                                                            \
	{                                                                                                                                      \
		if ((expression) != VK_SUCCESS) { throw std::runtime_error(error_message); }                                                       \
	}

struct DeletionQueue
{
public:
	void push(std::function<void()>&& function) { m_deletors.push_back(function); }
	void flush()
	{
		for (auto& m_deletor : std::ranges::reverse_view(m_deletors)) { m_deletor(); }
		m_deletors.clear();
	}

private:
	std::vector<std::function<void()>> m_deletors;
};

struct UploadContext
{
	vk::Fence uploadFence;
	vk::CommandPool commandPool;
};

#endif  // MORRIGU_VKTYPES_H
