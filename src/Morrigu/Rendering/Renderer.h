//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_RENDERER_H
#define MORRIGU_RENDERER_H

#include "Entity/Components/MeshRenderer.h"
#include "Entity/Entity.h"
#include "Events/ApplicationEvent.h"
#include "Rendering/Camera.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/RendererTypes.h"
#include "Rendering/Texture.h"
#include "Utils/Commands.h"

#include <GLFW/glfw3.h>

#include <ranges>
#include <string>
#include <vector>

namespace MRG
{
	struct RendererSpecification
	{
		std::string applicationName{"Morrigu engine application"};
		vk::PresentModeKHR preferredPresentMode{vk::PresentModeKHR::eMailbox};
		int windowWidth{1280};
		int windowHeight{720};

		// Default to an empty descriptor set
		vk::DescriptorSetLayoutCreateInfo level1SetInfo{
		  .bindingCount = 0,
		};
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
				cmdBuffer.copyBuffer(stagingBuffer.vkHandle, mesh->vertexBuffer.vkHandle, copy);
			});
		}

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
		[[nodiscard]] Components::MeshRenderer<VertexType>::VulkanObjects createMeshRenderer(const Ref<Mesh<VertexType>>& meshRef,
		                                                                                     const Ref<Material<VertexType>>& materialRef)
		{
			return
			  typename Components::MeshRenderer<VertexType>::VulkanObjects{m_device, m_allocator, meshRef, materialRef, defaultTexture};
		}

		[[nodiscard]] Ref<Framebuffer> createFrameBuffer(const FramebufferSpecification& fbSpec);

		[[nodiscard]] AllocatedBuffer createBuffer(std::size_t bufferSize, vk::BufferUsageFlags usage, VmaMemoryUsage allocationUsage);

		void bindBufferToSet1(AllocatedBuffer& buffer, std::size_t bufferSize);

		template<Vertex VertexType>
		void drawMeshes(const entt::registry& registry, const Camera& camera)
		{
			const auto& frameData = getCurrentFrameData();

			TimeData timeData{
			  // Shamelessly stolen from https://docs.unity3d.com/Manual/SL-UnityShaderVariables.html
			  .time = {elapsedTime / 20.f, elapsedTime, elapsedTime * 2.f, elapsedTime * 3.f},
			};
			void* data;
			vmaMapMemory(m_allocator, frameData.timeDataBuffer.allocation, &data);
			memcpy(data, &timeData, sizeof(TimeData));
			vmaUnmapMemory(m_allocator, frameData.timeDataBuffer.allocation);

			vk::Pipeline currentMaterial{};
			bool isFirst = true;
			auto view    = registry.view<Components::MeshRenderer<VertexType>>();
			for (const auto& entity : view) {
				auto [mrc] = view.get(entity);
				if (!mrc.isVisible) { continue; }
				if (isFirst) {
					frameData.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
					                                           mrc.material->pipelineLayout,
					                                           0,
					                                           {frameData.level0Descriptor, m_level1Descriptor},
					                                           {});
					isFirst = false;
				}
				if (currentMaterial != mrc.material->pipeline) {
					frameData.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mrc.material->pipeline);
					frameData.commandBuffer.setViewport(0,
					                                    vk::Viewport{
					                                      .x        = 0.f,
					                                      .y        = 0.f,
					                                      .width    = static_cast<float>(spec.windowWidth),
					                                      .height   = static_cast<float>(spec.windowHeight),
					                                      .minDepth = 0.f,
					                                      .maxDepth = 1.f,
					                                    });
					frameData.commandBuffer.setScissor(
					  0,
					  vk::Rect2D{
					    .offset{0, 0},
					    .extent = {static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight)},
					  });
					frameData.commandBuffer.bindDescriptorSets(
					  vk::PipelineBindPoint::eGraphics, mrc.material->pipelineLayout, 2, mrc.material->level2Descriptor, {});
					currentMaterial = mrc.material->pipeline;
				}

				if (mrc.material->shader->pcShaderStages != vk::ShaderStageFlags{}) {
					CameraData cameraData{
					  .viewProjectionMatrix = camera.getViewProjection(),
					  .worldPos             = camera.position,
					};
					frameData.commandBuffer.pushConstants(
					  mrc.material->pipelineLayout, mrc.material->shader->pcShaderStages, 0, sizeof(cameraData), &cameraData);
				}

				frameData.commandBuffer.bindDescriptorSets(
				  vk::PipelineBindPoint::eGraphics, mrc.material->pipelineLayout, 3, mrc.level3Descriptor, {});

				frameData.commandBuffer.bindVertexBuffers(0, mrc.mesh->vertexBuffer.vkHandle, {0});
				frameData.commandBuffer.draw(static_cast<uint32_t>(mrc.mesh->vertices.size()), 1, 0, 0);
			}
		}

		template<Vertex VertexType>
		void drawMeshes(const entt::registry& registry, const Camera& camera, Ref<Framebuffer> framebuffer)
		{
			framebuffer->commandBuffer.reset();
			vk::CommandBufferBeginInfo beginInfo{
			  .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
			};
			framebuffer->commandBuffer.begin(beginInfo);

			vk::ClearValue colorClearValue{};
			colorClearValue.color = {framebuffer->spec.clearColor};
			vk::ClearValue depthClearValue{};
			depthClearValue.depthStencil.depth = 1.f;

			std::array<vk::ClearValue, 2> clearValues{colorClearValue, depthClearValue};

			vk::RenderPassBeginInfo renderPassInfo{
			  .renderPass  = m_fbRenderPass,
			  .framebuffer = framebuffer->vkHandle,
			  .renderArea =
			    vk::Rect2D{
			      .offset = {0, 0},
			      .extent = {framebuffer->spec.width, framebuffer->spec.height},
			    },
			  .clearValueCount = static_cast<uint32_t>(clearValues.size()),
			  .pClearValues    = clearValues.data(),
			};
			framebuffer->commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

			TimeData timeData{
			  // Shamelessly stolen from https://docs.unity3d.com/Manual/SL-UnityShaderVariables.html
			  .time = {elapsedTime / 20.f, elapsedTime, elapsedTime * 2.f, elapsedTime * 3.f},
			};
			void* data;
			vmaMapMemory(m_allocator, framebuffer->timeDataBuffer.allocation, &data);
			memcpy(data, &timeData, sizeof(TimeData));
			vmaUnmapMemory(m_allocator, framebuffer->timeDataBuffer.allocation);

			vk::Pipeline currentMaterial{};
			bool isFirst = true;
			auto view    = registry.view<Components::MeshRenderer<VertexType>>();
			for (const auto& entity : view) {
				auto [mrc] = view.get(entity);
				if (!mrc.isVisible) { continue; }
				if (isFirst) {
					framebuffer->commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
					                                              mrc.material->pipelineLayout,
					                                              0,
					                                              {framebuffer->level0Descriptor, m_level1Descriptor},
					                                              {});
					isFirst = false;
				}
				if (currentMaterial != mrc.material->pipeline) {
					framebuffer->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mrc.material->pipeline);
					framebuffer->commandBuffer.setViewport(0,
					                                       vk::Viewport{
					                                         .x        = 0.f,
					                                         .y        = 0.f,
					                                         .width    = static_cast<float>(framebuffer->spec.width),
					                                         .height   = static_cast<float>(framebuffer->spec.height),
					                                         .minDepth = 0.f,
					                                         .maxDepth = 1.f,
					                                       });
					framebuffer->commandBuffer.setScissor(0,
					                                      vk::Rect2D{
					                                        .offset{0, 0},
					                                        .extent = {framebuffer->spec.width, framebuffer->spec.height},
					                                      });
					framebuffer->commandBuffer.bindDescriptorSets(
					  vk::PipelineBindPoint::eGraphics, mrc.material->pipelineLayout, 2, mrc.material->level2Descriptor, {});
					currentMaterial = mrc.material->pipeline;
				}

				if (mrc.material->shader->pcShaderStages != vk::ShaderStageFlags{}) {
					CameraData cameraData{
					  .viewProjectionMatrix = camera.getViewProjection(),
					  .worldPos             = glm::vec4{camera.position, 0.f},
					};
					framebuffer->commandBuffer.pushConstants(
					  mrc.material->pipelineLayout, mrc.material->shader->pcShaderStages, 0, sizeof(cameraData), &cameraData);
				}

				framebuffer->commandBuffer.bindDescriptorSets(
				  vk::PipelineBindPoint::eGraphics, mrc.material->pipelineLayout, 3, mrc.level3Descriptor, {});

				framebuffer->commandBuffer.bindVertexBuffers(0, mrc.mesh->vertexBuffer.vkHandle, {0});
				framebuffer->commandBuffer.draw(static_cast<uint32_t>(mrc.mesh->vertices.size()), 1, 0, 0);
			}

			framebuffer->commandBuffer.endRenderPass();
			framebuffer->commandBuffer.end();

			vk::SubmitInfo submitInfo{
			  .commandBufferCount = 1,
			  .pCommandBuffers    = &framebuffer->commandBuffer,
			};
			m_graphicsQueue.submit(submitInfo, framebuffer->renderFence);

			// Very wasteful, but easy and guarantees render order
			MRG_VK_CHECK_HPP(framebuffer->getVkDevice().waitForFences(framebuffer->renderFence, VK_TRUE, UINT64_MAX),
			                 "failed to wait for the framebuffer's wait semaphore!")
			framebuffer->getVkDevice().resetFences(framebuffer->renderFence);
		}

		[[nodiscard]] VmaAllocator getAllocator() { return m_allocator; }

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
		Ref<Shader> pbrShader{};
		Ref<Material<TexturedVertex>> pbrMaterial{};

		Ref<Texture> defaultTexture{};

	private:
		int m_frameNumber{0};

		static const constexpr int FRAMES_IN_FLIGHT = 1;
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

		vk::Queue m_graphicsQueue{};
		std::uint32_t m_graphicsQueueIndex{};

		vk::RenderPass m_renderPass{};
		vk::RenderPass m_fbRenderPass{};
		std::vector<vk::Framebuffer> m_framebuffers{};

		vk::PipelineCache m_pipelineCache{};

		vk::DescriptorSetLayout m_level0DSL{};
		vk::DescriptorSetLayout m_level1DSL{};
		vk::DescriptorSet m_level1Descriptor{};
		vk::DescriptorPool m_descriptorPool{};

		VmaAllocator m_allocator{};
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

		bool beginFrame();
		void endFrame();
		void beginImGui();
		void endImGui();

		void onResize();
	};
}  // namespace MRG

#endif  // MORRIGU_RENDERER_H
