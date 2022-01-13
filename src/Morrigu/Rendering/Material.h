//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_MATERIAL_H
#define MORRIGU_MATERIAL_H

#include "Rendering/Framebuffer.h"
#include "Rendering/PipelineBuilder.h"
#include "Rendering/RendererTypes.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Rendering/Vertex.h"

#include <type_traits>

namespace MRG
{
	// clang-format off
	template<typename T>
	concept NotPointer = !std::is_pointer_v<T>;
	// clang-format on

	struct CameraData
	{
		glm::mat4 viewProjectionMatrix;
		glm::vec4 worldPos;
	};

	struct MaterialConfiguration
	{
		bool zTest  = true;
		bool zWrite = true;
	};

	template<Vertex VertexType>
	class Material
	{
	public:
		explicit Material(vk::Device device,
		                  VmaAllocator allocator,
		                  const Ref<Shader>& shaderRef,
		                  vk::PipelineCache pipelineCache,
		                  vk::RenderPass renderPass,
		                  vk::DescriptorSetLayout level0DSL,
		                  vk::DescriptorSetLayout level1DSL,
		                  const Ref<Texture>& defaultTexture,
		                  const MaterialConfiguration& config)
		    : shader{shaderRef}, m_device{device}, m_allocator{allocator}
		{
			// Pool creation
			std::array<vk::DescriptorPoolSize, 2> sizes{
			  vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, std::max(static_cast<uint32_t>(shader->l2UBOData.size()), 1u)},
			  vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage,
			                         std::max(static_cast<uint32_t>(shader->l2ImageBindings.size()), 1u)},
			};
			vk::DescriptorPoolCreateInfo poolInfo{
			  .maxSets       = 1,
			  .poolSizeCount = static_cast<uint32_t>(sizes.size()),
			  .pPoolSizes    = sizes.data(),
			};

			m_descriptorPool = m_device.createDescriptorPool(poolInfo);

			vk::DescriptorSetAllocateInfo setAllocInfo{
			  .descriptorPool     = m_descriptorPool,
			  .descriptorSetCount = 1,
			  .pSetLayouts        = &shader->level2DSL,
			};
			level2Descriptor = m_device.allocateDescriptorSets(setAllocInfo).back();

			for (const auto& [bindingSlot, bindingInfo] : shader->l2UBOData) {
				AllocatedBuffer newBuffer{
				  m_allocator, bindingInfo.size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU};

				vk::DescriptorBufferInfo descriptorBufferInfo{
				  .buffer = newBuffer.vkHandle,
				  .offset = 0,
				  .range  = bindingInfo.size,
				};
				vk::WriteDescriptorSet setWrite{
				  .dstSet          = level2Descriptor,
				  .dstBinding      = bindingSlot,
				  .descriptorCount = 1,
				  .descriptorType  = vk::DescriptorType::eUniformBuffer,
				  .pBufferInfo     = &descriptorBufferInfo,
				};

				m_device.updateDescriptorSets(setWrite, {});
				uniformBuffers.insert(std::make_pair(bindingSlot, std::move(newBuffer)));
			}

			for (const auto& imageBinding : shader->l2ImageBindings) {
				sampledImages.insert(std::make_pair(imageBinding.first, defaultTexture));
				bindTexture(imageBinding.first, defaultTexture);
			}

			const bool hasPushConstants = shader->pcShaderStages != vk::ShaderStageFlags{};
			vk::PushConstantRange pushConstantRange{
			  .stageFlags = shader->pcShaderStages,
			  .offset     = 0,
			  .size       = sizeof(CameraData),
			};

			std::array<vk::DescriptorSetLayout, 4> setLayouts{level0DSL, level1DSL, shader->level2DSL, shader->level3DSL};
			vk::PipelineLayoutCreateInfo layoutInfo{
			  .setLayoutCount         = static_cast<uint32_t>(setLayouts.size()),
			  .pSetLayouts            = setLayouts.data(),
			  .pushConstantRangeCount = static_cast<uint32_t>(hasPushConstants ? 1 : 0),
			  .pPushConstantRanges    = &pushConstantRange,
			};
			pipelineLayout = m_device.createPipelineLayout(layoutInfo);

			const VertexInputDescription vertexInfo = VertexType::getVertexDescription();
			vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
			  .vertexBindingDescriptionCount   = static_cast<uint32_t>(vertexInfo.bindings.size()),
			  .pVertexBindingDescriptions      = vertexInfo.bindings.data(),
			  .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInfo.attributes.size()),
			  .pVertexAttributeDescriptions    = vertexInfo.attributes.data(),
			};

			vk::PipelineShaderStageCreateInfo vertStage{
			  .stage  = vk::ShaderStageFlagBits::eVertex,
			  .module = shader->vertexShaderModule,
			  .pName  = "main",
			};
			vk::PipelineShaderStageCreateInfo fragStage{
			  .stage  = vk::ShaderStageFlagBits::eFragment,
			  .module = shader->fragmentShaderModule,
			  .pName  = "main",
			};

			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
			  .topology = vk::PrimitiveTopology::eTriangleList,
			};

			vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
			  .polygonMode = vk::PolygonMode::eFill,
			  .cullMode    = vk::CullModeFlagBits::eNone,
			  .frontFace   = vk::FrontFace::eClockwise,
			  .lineWidth   = 1.f,
			};

			vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
			  .rasterizationSamples = vk::SampleCountFlagBits::e1,
			  .minSampleShading     = 1.f,
			};

			vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
			  .depthTestEnable  = static_cast<vk::Bool32>(config.zTest),
			  .depthWriteEnable = static_cast<vk::Bool32>(config.zWrite),
			  .depthCompareOp   = vk::CompareOp::eLessOrEqual,
			  .minDepthBounds   = 0.f,
			  .maxDepthBounds   = 1.f,
			};

			vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{
			  .blendEnable         = VK_TRUE,
			  .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			  .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			  .colorBlendOp        = vk::BlendOp::eAdd,
			  .srcAlphaBlendFactor = vk::BlendFactor::eOne,
			  .dstAlphaBlendFactor = vk::BlendFactor::eZero,
			  .alphaBlendOp        = vk::BlendOp::eAdd,
			  .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
			                    vk::ColorComponentFlagBits::eA,
			};

			PipelineBuilder pipelineBuilder{
			  .shaderStages{vertStage, fragStage},
			  .vertexInputInfo{vertexInputStateCreateInfo},
			  .inputAssemblyInfo{inputAssemblyStateCreateInfo},
			  .rasterizerInfo{rasterizationStateCreateInfo},
			  .multisamplingInfo{multisampleStateCreateInfo},
			  .depthStencilStateCreateInfo{depthStencilStateCreateInfo},
			  .colorBlendAttachment{colorBlendAttachmentState},
			  .pipelineLayout{pipelineLayout},
			  .pipelineCache{pipelineCache},
			};

			pipeline = pipelineBuilder.build_pipeline(m_device, renderPass);
		}

		~Material()
		{
			m_device.destroyPipeline(pipeline);
			m_device.destroyPipelineLayout(pipelineLayout);
			m_device.destroyDescriptorPool(m_descriptorPool);
		}

		[[nodiscard]] const Shader::Root& getUniformInfo(uint32_t bindingSlot) const
		{
			MRG_ENGINE_ASSERT(shader->l2UBOData.contains(bindingSlot), "Invalid binding slot!")
			return shader->l2UBOData.at(bindingSlot);
		}

		void uploadUniform(uint32_t bindingSlot, const NotPointer auto& uniformData)
		{
			MRG_ENGINE_ASSERT(uniformBuffers.contains(bindingSlot), "Invalid binding slot!")
			void* data;
			vmaMapMemory(m_allocator, uniformBuffers[bindingSlot].allocation, &data);
			memcpy(data, &uniformData, sizeof(uniformData));
			vmaUnmapMemory(m_allocator, uniformBuffers[bindingSlot].allocation);
		}

		void uploadUniform(uint32_t bindingSlot, void* srcData, std::size_t size)
		{
			MRG_ENGINE_ASSERT(uniformBuffers.contains(bindingSlot), "Invalid binding slot!")
			void* dstData;
			vmaMapMemory(m_allocator, uniformBuffers[bindingSlot].allocation, &dstData);
			memcpy(dstData, srcData, size);
			vmaUnmapMemory(m_allocator, uniformBuffers[bindingSlot].allocation);
		}

		void bindTexture(uint32_t bindingSlot, const Ref<Texture>& texture)
		{
			MRG_ENGINE_ASSERT(sampledImages.contains(bindingSlot), "Invalid binding slot!")
			vk::DescriptorImageInfo imageBufferInfo{
			  .sampler     = texture->sampler,
			  .imageView   = texture->image.view,
			  .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};
			vk::WriteDescriptorSet textureUpdate = {
			  .dstSet          = level2Descriptor,
			  .dstBinding      = bindingSlot,
			  .descriptorCount = 1,
			  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo      = &imageBufferInfo,
			};
			m_device.updateDescriptorSets(textureUpdate, {});

			sampledImages.at(bindingSlot) = texture;
		}

		// Uses the color attachment of the framebuffer as a texture
		void bindTexture(uint32_t bindingSlot, const Ref<Framebuffer>& framebuffer)
		{
			MRG_ENGINE_ASSERT(sampledImages.contains(bindingSlot), "Invalid binding slot!")
			vk::DescriptorImageInfo imageBufferInfo{
			  .sampler     = framebuffer->sampler,
			  .imageView   = framebuffer->colorImage.view,
			  .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};
			vk::WriteDescriptorSet textureUpdate = {
			  .dstSet          = level2Descriptor,
			  .dstBinding      = bindingSlot,
			  .descriptorCount = 1,
			  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo      = &imageBufferInfo,
			};
			m_device.updateDescriptorSets(textureUpdate, {});
		}

		vk::Pipeline pipeline;
		vk::PipelineLayout pipelineLayout;
		vk::DescriptorSet level2Descriptor;

		Ref<Shader> shader;

		std::map<uint32_t, AllocatedBuffer> uniformBuffers;
		std::map<uint32_t, Ref<Texture>> sampledImages;

	private:
		vk::Device m_device;
		VmaAllocator m_allocator;
		vk::DescriptorPool m_descriptorPool;
	};
}  // namespace MRG

#endif  // MORRIGU_MATERIAL_H
