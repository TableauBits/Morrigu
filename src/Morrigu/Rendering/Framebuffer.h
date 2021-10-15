//
// Created by Mathis Lamidey on 2021-09-30.
//

#ifndef MORRIGU_FRAMEBUFFER_H
#define MORRIGU_FRAMEBUFFER_H

#include "Rendering/RendererTypes.h"

#include <imgui.h>

#include <array>

namespace MRG
{
	struct FramebufferSpecification
	{
		uint32_t width;
		uint32_t height;

		std::array<float, 4> clearColor{0.f, 0.f, 0.f, 1.f};

		vk::Filter samplingFilter;
		vk::SamplerAddressMode samplingAddressMode;
	};

	class Framebuffer
	{
	public:
		struct VulkanObjects
		{
			vk::Device device;
			vk::Queue graphicsQueue;
			UploadContext uploadContext;
			VmaAllocator allocator;
			vk::Format swapchainFormat;
			vk::Format depthImageFormat;
			vk::RenderPass renderPass;
			uint32_t graphicsQueueIndex;
			vk::DescriptorSetLayout level0DSL;
		};

		Framebuffer(const FramebufferSpecification& specification, const VulkanObjects vkObjs);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&)      = default;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = default;

		~Framebuffer();

		void resize(uint32_t width, uint32_t height);
		void invalidate();

		[[nodiscard]] ImTextureID getImTexID();
		[[nodiscard]] vk::Device getVkDevice() const { return m_objects.device; }

		FramebufferSpecification spec;

		// Color attachment
		AllocatedImage colorImage{};

		// Depth attachment
		AllocatedImage depthImage{};

		vk::Sampler sampler{};
		vk::Framebuffer vkHandle{};

		// Render data
		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		vk::Fence renderFence{};

		vk::DescriptorPool descriptorPool{};
		vk::DescriptorSet level0Descriptor{};

		AllocatedBuffer timeDataBuffer{};

	private:
		void build();
		void destroy();

		ImTextureID m_imTexID{nullptr};
		VulkanObjects m_objects;
	};
}  // namespace MRG

#endif
