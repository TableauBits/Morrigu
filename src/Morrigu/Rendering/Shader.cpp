//
// Created by Mathis Lamidey on 2021-05-28.
//

#include "Shader.h"

#include "Core/FileNames.h"

#include <filesystem>
#include <fstream>

namespace MRG
{
	Shader::Shader(vk::Device device, const char* vertexShaderName, const char* fragmentShaderName, DeletionQueue& deletionQueue)
	    : m_device(device)
	{
		auto vertSrc = readSource(vertexShaderName);
		auto fragSrc = readSource(fragmentShaderName);

		vertexShaderModule   = loadShaderModule(vertSrc);
		fragmentShaderModule = loadShaderModule(fragSrc);

		deletionQueue.push([this]() {
			m_device.destroyShaderModule(vertexShaderModule);
			m_device.destroyShaderModule(fragmentShaderModule);
		});

		const auto vertexCompiler   = spirv_cross::Compiler{vertSrc};
		const auto fragmentCompiler = spirv_cross::Compiler{fragSrc};

		std::map<uint32_t, vk::DescriptorSetLayoutBinding> uboBindings{};
		std::map<uint32_t, vk::DescriptorSetLayoutBinding> sampledImagesBindings{};

		//// Vertex shader
		const auto& vertResources = vertexCompiler.get_shader_resources();

		/// descriptor sets
		// uniform buffers
		for (const auto& uniform : vertResources.uniform_buffers) {
			const auto setLevel    = vertexCompiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = vertexCompiler.get_decoration(uniform.id, spv::DecorationBinding);
			const auto uniformSize = vertexCompiler.get_declared_struct_size(vertexCompiler.get_type(uniform.type_id));

			// We are only interested in DS level 2: levels 0 and 1 do not vary by material
			if (setLevel == 2) {
				uboBindings.insert(std::make_pair(bindingSlot,
				                                  vk::DescriptorSetLayoutBinding{
				                                    .binding         = bindingSlot,
				                                    .descriptorType  = vk::DescriptorType::eUniformBuffer,
				                                    .descriptorCount = 1,
				                                    .stageFlags      = vk::ShaderStageFlagBits::eVertex,
				                                  }));
				uboSizes.insert(std::make_pair(bindingSlot, uniformSize));
			}
		}

		// sampled images (AKA textures)
		for (const auto& image : vertResources.sampled_images) {
			const auto setLevel    = vertexCompiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = vertexCompiler.get_decoration(image.id, spv::DecorationBinding);

			// We are only interested in DS level 2: levels 0 and 1 do not vary by material
			if (setLevel == 2) {
				sampledImagesBindings.insert(std::make_pair(bindingSlot,
				                                            vk::DescriptorSetLayoutBinding{
				                                              .binding         = bindingSlot,
				                                              .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
				                                              .descriptorCount = 1,
				                                              .stageFlags      = vk::ShaderStageFlagBits::eVertex,
				                                            }));
				imageBindings.emplace_back(bindingSlot);
			}
		}

		//// fragment shader
		const auto& fragResources = fragmentCompiler.get_shader_resources();

		/// descriptor sets
		// uniform buffers
		for (const auto& uniform : fragResources.uniform_buffers) {
			const auto setLevel    = fragmentCompiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = fragmentCompiler.get_decoration(uniform.id, spv::DecorationBinding);
			const auto uniformSize = vertexCompiler.get_declared_struct_size(vertexCompiler.get_type(uniform.type_id));

			// We are only interested in DS level 2: levels 0 and 1 do not vary by material
			if (setLevel == 2) {
				if (uboBindings.contains(bindingSlot)) {
					uboBindings.at(bindingSlot).stageFlags |= vk::ShaderStageFlagBits::eFragment;
				} else {
					uboBindings.insert(std::make_pair(bindingSlot,
					                                  vk::DescriptorSetLayoutBinding{
					                                    .binding         = bindingSlot,
					                                    .descriptorType  = vk::DescriptorType::eUniformBuffer,
					                                    .descriptorCount = 1,
					                                    .stageFlags      = vk::ShaderStageFlagBits::eFragment,
					                                  }));
					uboSizes.insert(std::make_pair(bindingSlot, uniformSize));
				}
			}
		}

		// sampled images (AKA textures)
		for (const auto& image : fragResources.sampled_images) {
			const auto setLevel    = fragmentCompiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = fragmentCompiler.get_decoration(image.id, spv::DecorationBinding);

			// We are only interested in DS level 2: levels 0 and 1 do not vary by material
			if (setLevel == 2) {
				if (sampledImagesBindings.contains(bindingSlot)) {
					sampledImagesBindings.at(bindingSlot).stageFlags |= vk::ShaderStageFlagBits::eFragment;
				} else {
					sampledImagesBindings.insert(std::make_pair(bindingSlot,
					                                            vk::DescriptorSetLayoutBinding{
					                                              .binding         = bindingSlot,
					                                              .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
					                                              .descriptorCount = 1,
					                                              .stageFlags      = vk::ShaderStageFlagBits::eFragment,
					                                            }));
					imageBindings.emplace_back(bindingSlot);
				}
			}
		}

		std::vector<vk::DescriptorSetLayoutBinding> finalBindings(uboBindings.size() + sampledImagesBindings.size());
		auto index = 0;
		for (const auto& ubo : uboBindings) { finalBindings[index++] = ubo.second; }
		for (const auto& sampledImage : sampledImagesBindings) { finalBindings[index++] = sampledImage.second; }

		vk::DescriptorSetLayoutCreateInfo setInfo{
		  .bindingCount = static_cast<uint32_t>(finalBindings.size()),
		  .pBindings    = finalBindings.data(),
		};

		level2DSL = m_device.createDescriptorSetLayout(setInfo);
		deletionQueue.push([this]() { m_device.destroyDescriptorSetLayout(level2DSL); });

		// push constants ranges
		const auto reflectPCRangesVert = vertexCompiler.get_active_buffer_ranges(vertResources.push_constant_buffers[0].id);
		const auto reflectPCRangesFrag = fragmentCompiler.get_active_buffer_ranges(fragResources.push_constant_buffers[0].id);
		pcRanges.resize(reflectPCRangesVert.size() + reflectPCRangesFrag.size());
		index = 0;
		for (const auto& range : reflectPCRangesVert) {
			pcRanges[index++] = vk::PushConstantRange{
			  .stageFlags = vk::ShaderStageFlagBits::eVertex,
			  .offset     = static_cast<uint32_t>(range.offset),
			  .size       = static_cast<uint32_t>(range.range),
			};
		}
		for (const auto& range : reflectPCRangesFrag) {
			pcRanges[index++] = vk::PushConstantRange{
			  .stageFlags = vk::ShaderStageFlagBits::eFragment,
			  .offset     = static_cast<uint32_t>(range.offset),
			  .size       = static_cast<uint32_t>(range.range),
			};
		}
	}

	std::vector<std::uint32_t> Shader::readSource(const char* filePath)
	{
		const auto completePath = Folders::Rendering::shadersFolder + filePath;
		MRG_ENGINE_ASSERT(std::filesystem::exists(completePath), "Shader file \"{}\" does not exists!", completePath)
		std::ifstream file{completePath, std::ios::binary | std::ios::ate};
		const auto fileSize = static_cast<std::size_t>(file.tellg());
		std::vector<std::uint32_t> buffer(fileSize / sizeof(std::uint32_t));
		file.seekg(std::ios::beg);
		file.read((char*)buffer.data(), static_cast<std::streamsize>(fileSize));
		file.close();

		return buffer;
	}

	vk::ShaderModule Shader::loadShaderModule(const std::vector<uint32_t>& src)
	{
		vk::ShaderModuleCreateInfo moduleInfo{
		  .codeSize = static_cast<std::uint32_t>(src.size() * sizeof(std::uint32_t)),
		  .pCode    = src.data(),
		};
		return m_device.createShaderModule(moduleInfo);
	}
}  // namespace MRG
