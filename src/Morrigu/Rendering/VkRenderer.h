//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_VKRENDERER_H
#define MORRIGU_VKRENDERER_H

#include "Events/ApplicationEvent.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderObject.h"
#include "Rendering/VkTypes.h"

#include <GLFW/glfw3.h>

#include <ranges>
#include <string>
#include <vector>

namespace MRG
{
	struct RendererSpecification
	{
		std::string applicationName;
		int windowWidth;
		int windowHeight;
		vk::PresentModeKHR preferredPresentMode;
	};

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

	struct GPUCameraData
	{
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;
	};

	struct FrameData
	{
		vk::Semaphore presentSemaphore, renderSemaphore;
		vk::Fence renderFence;

		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		AllocatedBuffer cameraBuffer;
		vk::DescriptorSet globalDescriptor;
	};

	class VkRenderer
	{
	public:
		void uploadMesh(Ref<Mesh>& mesh);

		RendererSpecification spec;

		bool isInitalized{false};
		int frameNumber{0};
		GLFWwindow* window{nullptr};

		glm::vec3 clearColor{};

		Ref<Material> defaultMaterial{};

	private:
		static const constexpr std::size_t FRAMES_IN_FLIGHT = 3;
		std::array<FrameData, FRAMES_IN_FLIGHT> m_framesData{};
		[[nodiscard]] FrameData& getCurrentFrameData() { return m_framesData[frameNumber % FRAMES_IN_FLIGHT]; }

		vk::Instance m_instance{};
		vk::DebugUtilsMessengerEXT m_debugMessenger{};
		vk::PhysicalDevice m_GPU{};
		vk::Device m_device{};
		vk::SurfaceKHR m_surface{};

		vk::SwapchainKHR m_swapchain{};
		vk::Format m_swapchainFormat{};
		std::vector<vk::Image> m_swapchainImages{};
		std::vector<vk::ImageView> m_swapchainImageViews{};
		uint32_t m_imageIndex{};

		AllocatedImage m_depthImage{};
		vk::ImageView m_depthImageView{};
		vk::Format m_depthFormat = vk::Format::eD32Sfloat;

		vk::Queue m_graphicsQueue{};
		std::uint32_t m_graphicsQueueIndex{};

		vk::RenderPass m_renderPass{};
		std::vector<vk::Framebuffer> m_framebuffers{};

		vk::PipelineCache m_pipelineCache{};

		vk::DescriptorSetLayout m_globalSetLayout{};
		vk::DescriptorPool m_descriptorPool{};

		VmaAllocator m_allocator{};
		DeletionQueue m_deletionQueue{};

		void initVulkan();
		void initSwapchain();
		void initCommands();
		void initDefaultRenderPass();
		void initFramebuffers();
		void initSyncSructs();
		void initDescriptors();
		void initMaterials();

		void destroySwapchain();

		[[nodiscard]] AllocatedBuffer createBuffer(std::size_t allocSize, vk::BufferUsageFlagBits bufferUsage, VmaMemoryUsage memoryUsage);

		/// Methods called by the application class
		friend class Application;
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame();
		void endFrame();

		void cleanup();

		void onResize();

		[[nodiscard]] vk::ShaderModule loadShaderModule(const char* filePath);
		void draw(const std::vector<Ref<RenderObject>>& drawables, const Camera& camera);
	};
}  // namespace MRG

#endif  // MORRIGU_VKRENDERER_H
