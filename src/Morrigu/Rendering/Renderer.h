//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_RENDERER_H
#define MORRIGU_RENDERER_H

#include "Events/ApplicationEvent.h"
#include "Rendering/Camera.h"
#include "Rendering/RendererTypes.h"
#include "Rendering/RenderObject.h"
#include "Rendering/Texture.h"

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

	struct CameraData
	{
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;
	};

	struct TimeData
	{
		glm::vec4 time;
	};

	struct FrameData
	{
		vk::Semaphore presentSemaphore, renderSemaphore;
		vk::Fence renderFence;

		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		AllocatedBuffer cameraBuffer;
		AllocatedBuffer timeDataBuffer;
		vk::DescriptorSet level0Descriptor;
	};

	class Renderer
	{
	public:
		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			const auto bufferSize = static_cast<uint32_t>(mesh->vertices.size() * sizeof(VertexType));

			// staging buffer
			VkBufferCreateInfo bufferInfo{
			  .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			  .size  = bufferSize,
			  .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			};

			VmaAllocationCreateInfo allocationInfo{
			  .usage = VMA_MEMORY_USAGE_CPU_ONLY,
			};

			AllocatedBuffer stagingBuffer;
			VkBuffer newRawBuffer;
			MRG_VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &allocationInfo, &newRawBuffer, &stagingBuffer.allocation, nullptr),
			             "Failed to allocate new buffer!")
			stagingBuffer.buffer = newRawBuffer;

			void* data;
			vmaMapMemory(m_allocator, stagingBuffer.allocation, &data);
			memcpy(data, mesh->vertices.data(), mesh->vertices.size() * sizeof(VertexType));
			vmaUnmapMemory(m_allocator, stagingBuffer.allocation);

			// mesh vertex buffer
			mesh->vertexBuffer =
			  Utils::Allocators::createBuffer(m_allocator,
			                                  bufferSize,
			                                  vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			                                  VMA_MEMORY_USAGE_GPU_ONLY,
			                                  m_deletionQueue);

			Utils::Allocators::immediateSubmit(m_device, m_graphicsQueue, m_uploadContext, [=](vk::CommandBuffer cmdBuffer) {
				vk::BufferCopy copy{
				  .size = bufferSize,
				};
				cmdBuffer.copyBuffer(stagingBuffer.buffer, mesh->vertexBuffer.buffer, copy);
			});

			vmaDestroyBuffer(m_allocator, stagingBuffer.buffer, stagingBuffer.allocation);
		}

		[[nodiscard]] Ref<Shader> createShader(const char* vertexShaderName, const char* fragmentShaderName);
		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(const Ref<Shader>& shader)
		{
			return createRef<Material<VertexType>>(
			  m_device, m_allocator, shader, m_pipelineCache, m_renderPass, m_level0DSL, m_level1DSL, defaultTexture, m_deletionQueue);
		}
		[[nodiscard]] Ref<Texture> createTexture(const char* fileName);
		template<Vertex VertexType>
		[[nodiscard]] Ref<RenderObject<VertexType>> createRenderObject(const Ref<Mesh<VertexType>>& mesh,
		                                                               const Ref<Material<VertexType>>& material)
		{
			return createRef<RenderObject<VertexType>>(m_device, m_allocator, mesh, material, defaultTexture, m_deletionQueue);
		}

		RendererSpecification spec;

		bool isInitalized{false};
		float elapsedTime;
		GLFWwindow* window{nullptr};

		glm::vec3 clearColor{};

		Ref<Shader> defaultBasicShader{};
		Ref<Material<BasicVertex>> defaultBasicMaterial{};
		Ref<Shader> defaultColoredShader{};
		Ref<Material<ColoredVertex>> defaultColoredMaterial{};
		Ref<Shader> defaultTexturedShader{};
		Ref<Material<TexturedVertex>> defaultTexturedMaterial{};

		Ref<Texture> defaultTexture{};

	private:
		int m_frameNumber{0};

		static const constexpr std::size_t FRAMES_IN_FLIGHT = 3;
		std::array<FrameData, FRAMES_IN_FLIGHT> m_framesData{};
		[[nodiscard]] FrameData& getCurrentFrameData() { return m_framesData[m_frameNumber % FRAMES_IN_FLIGHT]; }

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
		vk::DescriptorSet m_level1Descriptor{};
		vk::DescriptorPool m_descriptorPool{};

		VmaAllocator m_allocator{};
		DeletionQueue m_deletionQueue{};
		UploadContext m_uploadContext{};

		// ImGui data
		uint32_t m_imageCount{};
		vk::DescriptorPool m_imGuiPool{};

		void initVulkan();
		void initSwapchain();
		void initCommands();
		void initDefaultRenderPass();
		void initFramebuffers();
		void initSyncSructs();
		void initDescriptors();
		void initAssets();
		void initMaterials();
		void initImGui();

		void destroySwapchain();

		/// Methods called by the application class
		friend class Application;
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame();
		void endFrame();
		void beginImGui();
		void endImGui();

		void cleanup();

		void onResize();

		void draw(const std::vector<RenderData>& drawables, const Camera& camera);
	};
}  // namespace MRG

#endif  // MORRIGU_RENDERER_H
