//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_RENDERER_H
#define MORRIGU_RENDERER_H

#include "Events/ApplicationEvent.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderObject.h"
#include "Rendering/RendererTypes.h"
#include "Rendering/Texture.h"
#include "Rendering/UI/Font.h"
#include "Utils/Commands.h"

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

		AllocatedBuffer timeDataBuffer{};
		vk::DescriptorSet level0Descriptor;
	};

	class Renderer
	{
	public:
		Renderer(const RendererSpecification&, GLFWwindow*);
		~Renderer();

		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			const auto bufferSize = static_cast<uint32_t>(mesh->vertices.size() * sizeof(VertexType));

			AllocatedBuffer stagingBuffer{m_allocator, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY};

			void* data;
			vmaMapMemory(m_allocator, stagingBuffer.allocation, &data);
			memcpy(data, mesh->vertices.data(), mesh->vertices.size() * sizeof(VertexType));
			vmaUnmapMemory(m_allocator, stagingBuffer.allocation);

			// mesh vertex buffer
			mesh->vertexBuffer = AllocatedBuffer{m_allocator,
			                                     bufferSize,
			                                     vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			                                     VMA_MEMORY_USAGE_GPU_ONLY};

			Utils::Commands::immediateSubmit(m_device, m_graphicsQueue, m_uploadContext, [&](vk::CommandBuffer cmdBuffer) {
				vk::BufferCopy copy{
				  .size = bufferSize,
				};
				cmdBuffer.copyBuffer(stagingBuffer.buffer, mesh->vertexBuffer.buffer, copy);
			});
		}

		Ref<UI::Font> createFont(const std::string& fontPath);

		[[nodiscard]] Ref<Shader> createShader(const char* vertexShaderName, const char* fragmentShaderName);
		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(const Ref<Shader>& shader, const MaterialConfiguration& config)
		{
			return createRef<Material<VertexType>>(
			  m_device, m_allocator, shader, m_pipelineCache, m_renderPass, m_level0DSL, m_level1DSL, defaultTexture, config);
		}

		[[nodiscard]] Ref<Texture> createTexture(void* data, uint32_t width, uint32_t height);

		[[nodiscard]] Ref<Texture> createTexture(const char* fileName);

		template<Vertex VertexType>
		[[nodiscard]] Ref<RenderObject<VertexType>> createRenderObject(const Ref<Mesh<VertexType>>& mesh,
		                                                               const Ref<Material<VertexType>>& material)
		{
			return createRef<RenderObject<VertexType>>(m_device, m_allocator, mesh, material, defaultTexture);
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
		Ref<Material<TexturedVertex>> defaultUITexturedMaterial{};
		Ref<Shader> textShader{};
		Ref<Material<TexturedVertex>> textUIMaterial{};

		Ref<Texture> defaultTexture{};

	private:
		int m_frameNumber{0};

		static const constexpr int FRAMES_IN_FLIGHT = 3;
		std::array<FrameData, FRAMES_IN_FLIGHT> m_framesData{};
		[[nodiscard]] FrameData& getCurrentFrameData() { return m_framesData[static_cast<std::size_t>(m_frameNumber % FRAMES_IN_FLIGHT)]; }

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
		UploadContext m_uploadContext{};

		FT_Library m_ftHandle{};

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
		void initUI();

		void destroySwapchain();

		/// Methods called by the application class
		friend class Application;

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
