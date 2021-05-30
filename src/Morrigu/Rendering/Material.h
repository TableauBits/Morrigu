//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_MATERIAL_H
#define MORRIGU_MATERIAL_H

#include "Rendering/PipelineBuilder.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VkInitialize.h"
#include "Rendering/VkTypes.h"

namespace MRG
{
	template<Vertex VertexType>
	class Material
	{
	public:
		explicit Material(vk::Device device,
		                  VmaAllocator allocator,
		                  const Ref<Shader>& shader,
		                  vk::PipelineCache pipelineCache,
		                  vk::RenderPass renderPass,
		                  vk::DescriptorSetLayout level0DSL,
		                  vk::DescriptorSetLayout level1DSL,
		                  DeletionQueue& deletionQueue)
		    : m_allocator{allocator}, m_shader{shader}
		{
			// @TODO(Ithyx): replace with device limit
			constexpr static const auto ARBITRARY_UNIFORM_LIMIT = 72;

			// Pool creation
			std::array<vk::DescriptorPoolSize, 1> sizes{{vk::DescriptorType::eUniformBuffer, ARBITRARY_UNIFORM_LIMIT}};
			vk::DescriptorPoolCreateInfo poolInfo{
			  .maxSets       = ARBITRARY_UNIFORM_LIMIT,
			  .poolSizeCount = static_cast<uint32_t>(sizes.size()),
			  .pPoolSizes    = sizes.data(),
			};

			m_descriptorPool = device.createDescriptorPool(poolInfo);
			deletionQueue.push([this, device]() { device.destroyDescriptorPool(m_descriptorPool); });

			vk::DescriptorSetAllocateInfo setAllocInfo{
			  .descriptorPool     = m_descriptorPool,
			  .descriptorSetCount = 1,
			  .pSetLayouts        = &shader->level2DSL,
			};
			level2Descriptor = device.allocateDescriptorSets(setAllocInfo).back();

			for (const auto& [bindingSlot, size] : shader->uboSizes) {
				VkBufferCreateInfo bufferInfo{
				  .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				  .size  = size,
				  .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				};

				VmaAllocationCreateInfo allocationCreateInfo{.usage = VMA_MEMORY_USAGE_CPU_TO_GPU};

				AllocatedBuffer newBuffer;
				VkBuffer newRawBuffer;
				MRG_VK_CHECK(
				  vmaCreateBuffer(m_allocator, &bufferInfo, &allocationCreateInfo, &newRawBuffer, &newBuffer.allocation, nullptr),
				  "Failed to allocate new buffer!")
				newBuffer.buffer = newRawBuffer;
				m_uniformBuffers.insert(std::make_pair(bindingSlot, newBuffer));

				vk::DescriptorBufferInfo descriptorBufferInfo{
				  .buffer = newBuffer.buffer,
				  .offset = 0,
				  .range  = size,
				};
				vk::WriteDescriptorSet setWrite{
				  .dstSet          = level2Descriptor,
				  .dstBinding      = bindingSlot,
				  .descriptorCount = 1,
				  .descriptorType  = vk::DescriptorType::eUniformBuffer,
				  .pBufferInfo     = &descriptorBufferInfo,
				};

				device.updateDescriptorSets(setWrite, {});
			}
			deletionQueue.push([this]() {
				for (auto& [_, uniform] : m_uniformBuffers) { vmaDestroyBuffer(m_allocator, uniform.buffer, uniform.allocation); }
			});

			std::array<vk::DescriptorSetLayout, 3> setLayouts{level0DSL, level1DSL, shader->level2DSL};
			vk::PipelineLayoutCreateInfo layoutInfo{
			  .setLayoutCount         = static_cast<uint32_t>(setLayouts.size()),
			  .pSetLayouts            = setLayouts.data(),
			  .pushConstantRangeCount = static_cast<uint32_t>(shader->pcRanges.size()),
			  .pPushConstantRanges    = shader->pcRanges.data(),
			};
			pipelineLayout = device.createPipelineLayout(layoutInfo);
			deletionQueue.push([this, device]() { device.destroyPipelineLayout(pipelineLayout); });

			const VertexInputDescription vertexInfo = VertexType::getVertexDescription();
			vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
			  .vertexBindingDescriptionCount   = static_cast<uint32_t>(vertexInfo.bindings.size()),
			  .pVertexBindingDescriptions      = vertexInfo.bindings.data(),
			  .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInfo.attributes.size()),
			  .pVertexAttributeDescriptions    = vertexInfo.attributes.data(),
			};

			PipelineBuilder pipelineBuilder{
			  .shaderStages{VkInit::pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eVertex, shader->vertexShaderModule),
			                VkInit::pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eFragment, shader->fragmentShaderModule)},
			  .vertexInputInfo{vertexInputStateCreateInfo},
			  .inputAssemblyInfo{VkInit::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList)},
			  .rasterizerInfo{VkInit::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill)},
			  .multisamplingInfo{VkInit::pipelineMultisampleStateCreateInfo()},
			  .depthStencilStateCreateInfo{VkInit::pipelineDepthStencilStateCreateInfo(true, true, vk::CompareOp::eLessOrEqual)},
			  .colorBlendAttachment{VkInit::pipelineColorBlendAttachmentState()},
			  .pipelineLayout{pipelineLayout},
			  .pipelineCache{pipelineCache},
			};

			pipeline = pipelineBuilder.build_pipeline(device, renderPass);
			deletionQueue.push([this, device]() { device.destroyPipeline(pipeline); });
		}

		template<typename UniformType>
		void uploadUniform(uint32_t bindingSlot, const UniformType& uniformData)
		{
			MRG_ENGINE_ASSERT(m_uniformBuffers.contains(bindingSlot), "Invalid binding slot!")
			MRG_ENGINE_ASSERT(sizeof(UniformType) == m_shader->uboSizes[bindingSlot], "Specified uniform's size does not match!")
			void* data;
			vmaMapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation, &data);
			memcpy(data, &uniformData, sizeof(UniformType));
			vmaUnmapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation);
		}

		vk::Pipeline pipeline;
		vk::PipelineLayout pipelineLayout;
		vk::DescriptorSet level2Descriptor;

	private:
		VmaAllocator m_allocator;

		Ref<Shader> m_shader;
		vk::DescriptorPool m_descriptorPool;

		std::map<uint32_t, AllocatedBuffer> m_uniformBuffers;
	};
}  // namespace MRG

#endif  // MORRIGU_MATERIAL_H
