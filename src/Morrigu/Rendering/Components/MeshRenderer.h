//
// Created by Mathis Lamidey on 2021-11-11
//

#ifndef MORRIGU_COMP_MESH_RENDERER_H
#define MORRIGU_COMP_MESH_RENDERER_H

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include "Rendering/Vertex.h"
#include "Utils/GLMIncludeHelper.h"

namespace MRG::Components
{
	template<Vertex VertexType>
	class MeshRenderer
	{
	public:
		struct VulkanObjects
		{
			vk::Device device;
			VmaAllocator allocator;
			const Ref<Mesh<VertexType>>& meshRef;
			const Ref<Material<VertexType>>& materialRef;
			const Ref<Texture>& defaultTexture;
		};

		MeshRenderer(const VulkanObjects& objs)
		    : mesh{objs.meshRef}, material{objs.materialRef}, m_device{objs.device}, m_allocator{objs.allocator}
		{
			// Pool creation
			std::array<vk::DescriptorPoolSize, 2> sizes{
			  vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer,
			                         std::max(static_cast<uint32_t>(material->shader->l3UBOData.size()), 1u)},
			  vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage,
			                         std::max(static_cast<uint32_t>(material->shader->l3ImageBindings.size()), 1u)},
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
			  .pSetLayouts        = &material->shader->level3DSL,
			};
			level3Descriptor = m_device.allocateDescriptorSets(setAllocInfo).back();

			for (const auto& [bindingSlot, bindingInfo] : material->shader->l3UBOData) {
				AllocatedBuffer newBuffer{
				  m_allocator, bindingInfo.size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU};

				vk::DescriptorBufferInfo descriptorBufferInfo{
				  .buffer = newBuffer.vkHandle,
				  .offset = 0,
				  .range  = bindingInfo.size,
				};
				vk::WriteDescriptorSet setWrite{
				  .dstSet          = level3Descriptor,
				  .dstBinding      = bindingSlot,
				  .descriptorCount = 1,
				  .descriptorType  = vk::DescriptorType::eUniformBuffer,
				  .pBufferInfo     = &descriptorBufferInfo,
				};

				m_device.updateDescriptorSets(setWrite, {});
				m_uniformBuffers.insert(std::make_pair(bindingSlot, std::move(newBuffer)));
			}

			for (const auto& imageBinding : material->shader->l3ImageBindings) {
				m_sampledImages.insert(std::make_pair(imageBinding, objs.defaultTexture));
				bindTexture(imageBinding, objs.defaultTexture);
			}

			updateTransform(glm::mat4{1.f});
		}

		MeshRenderer(const MeshRenderer&) = delete;
		MeshRenderer(MeshRenderer&& other)
		{
			isVisible        = std::move(other.isVisible);
			mesh             = std::move(other.mesh);
			material         = std::move(other.material);
			level3Descriptor = std::move(other.level3Descriptor);

			m_device         = std::move(other.m_device);
			m_allocator      = std::move(other.m_allocator);
			m_descriptorPool = std::move(other.m_descriptorPool);
			m_uniformBuffers = std::move(other.m_uniformBuffers);
			m_sampledImages  = std::move(other.m_sampledImages);

			// Necessary to take ownership
			other.m_allocator = nullptr;
		}

		~MeshRenderer()
		{
			if (m_allocator != nullptr) m_device.destroyDescriptorPool(m_descriptorPool);
		}

		MeshRenderer& operator=(const MeshRenderer&) = delete;

		MeshRenderer& operator=(MeshRenderer&& other)
		{
			isVisible        = std::move(other.isVisible);
			mesh             = std::move(other.mesh);
			material         = std::move(other.material);
			level3Descriptor = std::move(other.level3Descriptor);

			m_device         = std::move(other.m_device);
			m_allocator      = std::move(other.m_allocator);
			m_descriptorPool = std::move(other.m_descriptorPool);
			m_uniformBuffers = std::move(other.m_uniformBuffers);
			m_sampledImages  = std::move(other.m_sampledImages);

			// Necessary to take ownership
			other.m_allocator = nullptr;

			return *this;
		}

		void uploadUniform(uint32_t bindingSlot, const NotPointer auto& uniformData)
		{
			MRG_ENGINE_ASSERT(m_uniformBuffers.contains(bindingSlot), "Invalid binding slot!")
			void* data;
			vmaMapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation, &data);
			memcpy(data, &uniformData, sizeof(uniformData));
			vmaUnmapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation);
		}

		void uploadUniform(uint32_t bindingSlot, void* srcData, std::size_t size)
		{
			MRG_ENGINE_ASSERT(m_uniformBuffers.contains(bindingSlot), "Invalid binding slot!")
			void* dstData;
			vmaMapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation, &dstData);
			memcpy(dstData, srcData, size);
			vmaUnmapMemory(m_allocator, m_uniformBuffers[bindingSlot].allocation);
		}

		void bindTexture(uint32_t bindingSlot, const Ref<Texture>& texture)
		{
			MRG_ENGINE_ASSERT(m_sampledImages.contains(bindingSlot), "Invalid binding slot!")
			vk::DescriptorImageInfo imageBufferInfo{
			  .sampler     = texture->sampler,
			  .imageView   = texture->image.view,
			  .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};
			vk::WriteDescriptorSet textureUpdate = {
			  .dstSet          = level3Descriptor,
			  .dstBinding      = bindingSlot,
			  .descriptorCount = 1,
			  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo      = &imageBufferInfo,
			};
			m_device.updateDescriptorSets(textureUpdate, {});

			m_sampledImages.at(bindingSlot) = texture;
		}

		// Uses the color attachment of the framebuffer as a texture
		void bindTexture(uint32_t bindingSlot, const Ref<Framebuffer>& framebuffer)
		{
			MRG_ENGINE_ASSERT(m_sampledImages.contains(bindingSlot), "Invalid binding slot!")
			vk::DescriptorImageInfo imageBufferInfo{
			  .sampler     = framebuffer->sampler,
			  .imageView   = framebuffer->colorImage.view,
			  .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};
			vk::WriteDescriptorSet textureUpdate = {
			  .dstSet          = level3Descriptor,
			  .dstBinding      = bindingSlot,
			  .descriptorCount = 1,
			  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo      = &imageBufferInfo,
			};
			m_device.updateDescriptorSets(textureUpdate, {});
		}

		void updateTransform(const glm::mat4& transform) { uploadUniform(0, transform); }

		bool isVisible{true};

		Ref<Mesh<VertexType>> mesh;
		Ref<Material<VertexType>> material;
		vk::DescriptorSet level3Descriptor;

	private:
		vk::Device m_device;
		VmaAllocator m_allocator;

		vk::DescriptorPool m_descriptorPool;

		std::map<uint32_t, AllocatedBuffer> m_uniformBuffers;
		std::map<uint32_t, Ref<Texture>> m_sampledImages;
	};
}  // namespace MRG::Components

#endif
