#include "DescriptorAllocator.h"

#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"

namespace MRG::Vulkan
{
	void DescriptorAllocator::init(const MRG::Vulkan::WindowProperties* data)
	{
		MRG_PROFILE_FUNCTION();

		m_data = data;

		createDescriptorPool();
	}

	void DescriptorAllocator::createDescriptorPool()
	{
		MRG_PROFILE_FUNCTION();

		VkDescriptorPool descriptorPool;

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = MAX_POOL_SIZE;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = MAX_POOL_SIZE;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = MAX_POOL_SIZE;

		MRG_VKVALIDATE(vkCreateDescriptorPool(m_data->device, &poolInfo, nullptr, &descriptorPool), "failed to create descriptor pool!");
		m_pools.emplace_back(descriptorPool);

		std::vector<VkDescriptorSet> descriptorSets(MAX_POOL_SIZE);

		std::vector<VkDescriptorSetLayout> layout(MAX_POOL_SIZE, m_data->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = MAX_POOL_SIZE;
		allocInfo.pSetLayouts = layout.data();

		MRG_VKVALIDATE(vkAllocateDescriptorSets(m_data->device, &allocInfo, descriptorSets.data()), "failed to allocate descriptor sets!");

		m_descriptorSets.emplace_back(descriptorSets);
	}

	[[nodiscard]] std::vector<VkDescriptorSet> DescriptorAllocator::requestDescriptorSets(const std::vector<SceneDrawCallInfo>& sceneInfo,
	                                                                                      uint32_t imageIndex,
	                                                                                      Ref<Texture2D> defaultTexture)
	{
		MRG_PROFILE_FUNCTION();

		if (sceneInfo.size() == 0)
			return {};

		// First, allocate the necessary descriptor pools to ensure that we can handle the demand
		std::size_t poolAllocsNeeded = 0;
		if (sceneInfo.size() > MAX_POOL_SIZE) {
			poolAllocsNeeded = (sceneInfo.size() / MAX_POOL_SIZE) - m_pools.size();
			if (sceneInfo.size() % MAX_POOL_SIZE != 0)
				++poolAllocsNeeded;
			for (std::size_t i = 0; i < poolAllocsNeeded; ++i) { createDescriptorPool(); }
		}

		// Next, update the needed descriptor sets and add them to a return list
		std::vector<VkDescriptorSet> returnedDescriptors(sceneInfo.size());
		for (std::size_t i = 0; i < sceneInfo.size(); ++i) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_data->uniformBuffers[imageIndex].handle;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(MRG::Vulkan::UniformBufferObject);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView =
			  sceneInfo[i].texture.value_or(std::make_pair<VkImageView, VkSampler>(defaultTexture->getImageView(), {})).first;
			imageInfo.sampler =
			  sceneInfo[i].texture.value_or(std::make_pair<VkImageView, VkSampler>({}, defaultTexture->getSampler())).second;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_descriptorSets[i / MAX_POOL_SIZE][i % MAX_POOL_SIZE];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_descriptorSets[i / MAX_POOL_SIZE][i % MAX_POOL_SIZE];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_data->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			returnedDescriptors[i] = m_descriptorSets[i / MAX_POOL_SIZE][i % MAX_POOL_SIZE];
		}

		return returnedDescriptors;
	}

}  // namespace MRG::Vulkan
