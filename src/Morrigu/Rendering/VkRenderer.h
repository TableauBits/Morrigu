//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_VKRENDERER_H
#define MORRIGU_VKRENDERER_H

#include "Events/ApplicationEvent.h"
#include "Rendering/VkTypes.h"

#include <GLFW/glfw3.h>

#include <string>

namespace MRG
{
	struct RendererSpecification
	{
		std::string applicationName;
		int windowWidth;
		int windowHeight;
		VkPresentModeKHR presentMode;
	};

	class VkRenderer
	{
	public:
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame();
		void endFrame();

		void cleanup();

		void onResize();

		[[nodiscard]] vk::ShaderModule loadShaderModule(const char* filePath);

		RendererSpecification spec;

		bool isInitalized{false};
		int frameNumber{0};
		GLFWwindow* window{nullptr};

	private:
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

		vk::Queue m_graphicsQueue{};
		std::uint32_t m_graphicsQueueIndex{};
		vk::CommandPool m_cmdPool{};
		vk::CommandBuffer m_mainCmdBuffer{};

		vk::RenderPass m_renderPass{};
		std::vector<vk::Framebuffer> m_framebuffers{};

		vk::Semaphore m_presentSemaphore{}, m_renderSemaphore{};
		vk::Fence m_renderFence{};

		vk::PipelineCache m_pipelineCache{};
		vk::PipelineLayout m_trianglePipelineLayout{};
		vk::Pipeline m_trianglePipeline{};

		void initVulkan();
		void initSwapchain();
		void initCommands();
		void initDefaultRenderPass();
		void initFramebuffers();
		void initSyncSructs();
		void initPipelines();

		void destroySwapchain();
	};
}  // namespace MRG

#endif  // MORRIGU_VKRENDERER_H
