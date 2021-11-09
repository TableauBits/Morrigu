//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_ENTITY_H
#define MORRIGU_ENTITY_H

#include "Rendering/Framebuffer.h"
#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

#include "Utils/GLMIncludeHelper.h"

#include <entt/entt.hpp>

#include <iterator>

namespace MRG
{
	struct RenderData
	{
		// mesh data
		vk::Buffer vertexBuffer;
		std::size_t vertexCount{};

		// material data
		vk::PipelineLayout materialPipelineLayout;
		vk::Pipeline materialPipeline;
		vk::DescriptorSet level2Descriptor;

		// Entity data
		vk::DescriptorSet level3Descriptor;
		bool isVisible{};
	};

	// clang-format off
	template<typename Iterator>
	concept EntityIterator =
		std::input_iterator<Iterator> &&
		requires(Iterator it) {
			{(*it)->getRenderData()} -> std::convertible_to<RenderData>;
		}
	;
	// clang-format on

	template<Vertex VertexType>
	class Entity
	{
	public:
		Entity(vk::Device device,
		       VmaAllocator allocator,
		       const Ref<Mesh<VertexType>>& meshRef,
		       const Ref<Material<VertexType>>& materialRef,
		       const Ref<Texture>& defaultTexture,
		       const Ref<entt::registry>& registryRef)
		    : mesh{meshRef}, material{materialRef}, m_registry{registryRef}, m_device{device}, m_allocator{allocator}
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
				  allocator, bindingInfo.size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU};

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
				m_sampledImages.insert(std::make_pair(imageBinding, defaultTexture));
				bindTexture(imageBinding, defaultTexture);
			}

			uploadPosition();

			m_id = m_registry->create();
		}

		~Entity()
		{
			m_registry->destroy(m_id);
			m_device.destroyDescriptorPool(m_descriptorPool);
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

		RenderData getRenderData()
		{
			return RenderData{
			  .vertexBuffer           = mesh->vertexBuffer.vkHandle,
			  .vertexCount            = mesh->vertices.size(),
			  .materialPipelineLayout = material->pipelineLayout,
			  .materialPipeline       = material->pipeline,
			  .level2Descriptor       = material->level2Descriptor,
			  .level3Descriptor       = level3Descriptor,
			  .isVisible              = isVisible,
			};
		}

		void rotate(const glm::vec3& axis, float radRotation)
		{
			modelMatrix = glm::rotate(modelMatrix, radRotation, axis);
			uploadPosition();
		}
		void scale(const glm::vec3& scaling)
		{
			modelMatrix = glm::scale(modelMatrix, scaling);
			uploadPosition();
		}
		void translate(const glm::vec3& translation)
		{
			modelMatrix = glm::translate(modelMatrix, translation);
			uploadPosition();
		}

		void uploadPosition() { uploadUniform(0, modelMatrix); }

		[[nodiscard]] entt::entity getID() const { return m_id; }
		void setVisible(bool visible) { *isVisible = visible; }

		Ref<Mesh<VertexType>> mesh;
		Ref<Material<VertexType>> material;
		glm::mat4 modelMatrix{1.f};
		bool isVisible{true};
		vk::DescriptorSet level3Descriptor;

	private:
		Ref<entt::registry> m_registry;
		entt::entity m_id;

		vk::Device m_device;
		VmaAllocator m_allocator;

		vk::DescriptorPool m_descriptorPool;

		std::map<uint32_t, AllocatedBuffer> m_uniformBuffers;
		std::map<uint32_t, Ref<Texture>> m_sampledImages;
	};
}  // namespace MRG

#endif  // MORRIGU_ENTITY_H
