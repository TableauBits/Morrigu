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

	struct UploadContext
	{
		vk::Fence uploadFence;
		vk::CommandPool commandPool;
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
		vk::DescriptorSet level0Descriptor;
	};

	class VkRenderer
	{
	public:
		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			const auto bufferSize = static_cast<uint32_t>(mesh->vertices.size() * sizeof(ColoredVertex));

			// staging buffer
			VkBufferCreateInfo stagingBufferInfo{
			  .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			  .size  = bufferSize,
			  .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			};
			VmaAllocationCreateInfo vmaStagingAllocationCreateInfo{
			  .usage = VMA_MEMORY_USAGE_CPU_ONLY,
			};
			VkBuffer rawBuffer{};
			AllocatedBuffer stagingBuffer{};
			MRG_VK_CHECK(
			  vmaCreateBuffer(
			    m_allocator, &stagingBufferInfo, &vmaStagingAllocationCreateInfo, &rawBuffer, &stagingBuffer.allocation, nullptr),
			  "Failed to allocate new buffer!")
			stagingBuffer.buffer = rawBuffer;

			void* data;
			vmaMapMemory(m_allocator, stagingBuffer.allocation, &data);
			memcpy(data, mesh->vertices.data(), mesh->vertices.size() * sizeof(ColoredVertex));
			vmaUnmapMemory(m_allocator, stagingBuffer.allocation);

			// mesh vertex buffer
			VkBufferCreateInfo vertexBufferInfo{
			  .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			  .size  = bufferSize,
			  .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			};
			VmaAllocationCreateInfo vmaVertexAllocationCreateInfo{
			  .usage = VMA_MEMORY_USAGE_GPU_ONLY,
			};
			MRG_VK_CHECK(
			  vmaCreateBuffer(
			    m_allocator, &vertexBufferInfo, &vmaVertexAllocationCreateInfo, &rawBuffer, &mesh->vertexBuffer.allocation, nullptr),
			  "Failed to allocate new buffer!")
			mesh->vertexBuffer.buffer = rawBuffer;

			immediateSubmit([=](vk::CommandBuffer cmdBuffer) {
				vk::BufferCopy copy{
				  .size = bufferSize,
				};
				cmdBuffer.copyBuffer(stagingBuffer.buffer, mesh->vertexBuffer.buffer, copy);
			});

			vmaDestroyBuffer(m_allocator, stagingBuffer.buffer, stagingBuffer.allocation);
			m_deletionQueue.push(
			  [this, mesh]() { vmaDestroyBuffer(m_allocator, mesh->vertexBuffer.buffer, mesh->vertexBuffer.allocation); });
		}

		[[nodiscard]] Ref<Shader> createShader(const char* vertexShaderName, const char* fragmentShaderName);

		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(Ref<Shader>& shader)
		{
			return createRef<Material<VertexType>>(
			  m_device, m_allocator, std::move(shader), m_pipelineCache, m_renderPass, m_level0DSL, m_level1DSL, m_deletionQueue);
		}

		RendererSpecification spec;

		bool isInitalized{false};
		int frameNumber{0};
		GLFWwindow* window{nullptr};

		glm::vec3 clearColor{};

		Ref<Shader> defaultColoredShader{};
		Ref<Material<ColoredVertex>> defaultColoredMaterial{};

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

		vk::DescriptorSetLayout m_level0DSL{};
		vk::DescriptorSetLayout m_level1DSL{};
		vk::DescriptorPool m_descriptorPool{};

		VmaAllocator m_allocator{};
		DeletionQueue m_deletionQueue{};
		UploadContext m_uploadContext{};

		void initVulkan();
		void initSwapchain();
		void initCommands();
		void initDefaultRenderPass();
		void initFramebuffers();
		void initSyncSructs();
		void initDescriptors();
		void initMaterials();

		void destroySwapchain();

		/// Helper methods
		[[nodiscard]] AllocatedBuffer createBuffer(std::size_t allocSize, vk::BufferUsageFlagBits bufferUsage, VmaMemoryUsage memoryUsage);
		void immediateSubmit(std::function<void(vk::CommandBuffer cmd)>&& function);

		/// Methods called by the application class
		friend class Application;
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame();
		void endFrame();

		void cleanup();

		void onResize();

		void draw(const std::vector<RenderData>& drawables, const Camera& camera);
	};
}  // namespace MRG

#endif  // MORRIGU_VKRENDERER_H
