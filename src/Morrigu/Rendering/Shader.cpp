//
// Created by Mathis Lamidey on 2021-05-28.
//

#include "Shader.h"

#include "Core/FileNames.h"

#include <filesystem>
#include <fstream>

namespace MRG
{
	Shader::Shader(vk::Device device, const char* vertexShaderName, const char* fragmentShaderName) : m_device(device)
	{
		auto vertSrc = readSource(vertexShaderName);
		auto fragSrc = readSource(fragmentShaderName);

		vertexShaderModule   = loadShaderModule(vertSrc);
		fragmentShaderModule = loadShaderModule(fragSrc);

		const auto vertexCompiler   = spirv_cross::Compiler{vertSrc};
		const auto fragmentCompiler = spirv_cross::Compiler{fragSrc};

		std::map<uint32_t, vk::DescriptorSetLayoutBinding> level2UBOBindings{};
		std::map<uint32_t, vk::DescriptorSetLayoutBinding> level2SampledImagesBindings{};

		std::map<uint32_t, vk::DescriptorSetLayoutBinding> level3UBOBindings{};
		std::map<uint32_t, vk::DescriptorSetLayoutBinding> level3SampledImagesBindings{};

		//// Vertex shader
		const auto& vertResources = vertexCompiler.get_shader_resources();

		/// descriptor sets
		// uniform buffers
		for (const auto& uniform : vertResources.uniform_buffers) {
			const auto setLevel    = vertexCompiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = vertexCompiler.get_decoration(uniform.id, spv::DecorationBinding);

			MRG_ENGINE_ASSERT(setLevel <= 3, "Invalid shader detected: only 4 sets are allowed!")

			// We are only interested in DS level 2 and 3: levels 0 and 1 do not vary by material
			if (setLevel >= 2) {
				auto& bindingsMap = (setLevel == 2) ? level2UBOBindings : level3UBOBindings;
				auto& sizeMap     = (setLevel == 2) ? l2UBOData : l3UBOData;

				bindingsMap.insert(std::make_pair(bindingSlot,
				                                  vk::DescriptorSetLayoutBinding{
				                                    .binding         = bindingSlot,
				                                    .descriptorType  = vk::DescriptorType::eUniformBuffer,
				                                    .descriptorCount = 1,
				                                    .stageFlags      = vk::ShaderStageFlagBits::eVertex,
				                                  }));

				const auto uniformData = populateUniformData(vertexCompiler, uniform);
				sizeMap.insert(std::make_pair(bindingSlot, uniformData));
			}
		}

		// sampled images (AKA textures)
		for (const auto& image : vertResources.sampled_images) {
			const auto setLevel    = vertexCompiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = vertexCompiler.get_decoration(image.id, spv::DecorationBinding);

			MRG_ENGINE_ASSERT(setLevel <= 3, "Invalid shader detected: only 4 sets are allowed!")

			// We are only interested in DS level 2 and 3: levels 0 and 1 do not vary by material
			if (setLevel >= 2) {
				auto& bindingsMap = (setLevel == 2) ? level2SampledImagesBindings : level3SampledImagesBindings;
				auto& sizeMap     = (setLevel == 2) ? l2ImageBindings : l3ImageBindings;

				bindingsMap.insert(std::make_pair(bindingSlot,
				                                  vk::DescriptorSetLayoutBinding{
				                                    .binding         = bindingSlot,
				                                    .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
				                                    .descriptorCount = 1,
				                                    .stageFlags      = vk::ShaderStageFlagBits::eVertex,
				                                  }));
				sizeMap.insert(std::make_pair(bindingSlot, TextureBindingInfo{image.name}));
			}
		}

		if (!vertResources.push_constant_buffers.empty()) { pcShaderStages |= vk::ShaderStageFlagBits::eVertex; }

		//// fragment shader
		const auto& fragResources = fragmentCompiler.get_shader_resources();

		/// descriptor sets
		// uniform buffers
		for (const auto& uniform : fragResources.uniform_buffers) {
			const auto setLevel    = fragmentCompiler.get_decoration(uniform.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = fragmentCompiler.get_decoration(uniform.id, spv::DecorationBinding);

			MRG_ENGINE_ASSERT(setLevel <= 3, "Invalid shader detected: only 4 sets are allowed!")

			// We are only interested in DS level 2 and 3: levels 0 and 1 do not vary by material
			if (setLevel >= 2) {
				auto& bindingsMap = (setLevel == 2) ? level2UBOBindings : level3UBOBindings;
				auto& sizeMap     = (setLevel == 2) ? l2UBOData : l3UBOData;

				if (bindingsMap.contains(bindingSlot)) {
					bindingsMap.at(bindingSlot).stageFlags |= vk::ShaderStageFlagBits::eFragment;
				} else {
					bindingsMap.insert(std::make_pair(bindingSlot,
					                                  vk::DescriptorSetLayoutBinding{
					                                    .binding         = bindingSlot,
					                                    .descriptorType  = vk::DescriptorType::eUniformBuffer,
					                                    .descriptorCount = 1,
					                                    .stageFlags      = vk::ShaderStageFlagBits::eFragment,
					                                  }));
					const auto uniformData = populateUniformData(fragmentCompiler, uniform);
					sizeMap.insert(std::make_pair(bindingSlot, uniformData));
				}
			}
		}

		// sampled images (AKA textures)
		for (const auto& image : fragResources.sampled_images) {
			const auto setLevel    = fragmentCompiler.get_decoration(image.id, spv::DecorationDescriptorSet);
			const auto bindingSlot = fragmentCompiler.get_decoration(image.id, spv::DecorationBinding);

			MRG_ENGINE_ASSERT(setLevel <= 3, "Invalid shader detected: only 4 sets are allowed!")

			// We are only interested in DS level 2 and 3: levels 0 and 1 do not vary by material
			if (setLevel >= 2) {
				auto& bindingsMap = (setLevel == 2) ? level2SampledImagesBindings : level3SampledImagesBindings;
				auto& sizeMap     = (setLevel == 2) ? l2ImageBindings : l3ImageBindings;

				if (bindingsMap.contains(bindingSlot)) {
					bindingsMap.at(bindingSlot).stageFlags |= vk::ShaderStageFlagBits::eFragment;
				} else {
					bindingsMap.insert(std::make_pair(bindingSlot,
					                                  vk::DescriptorSetLayoutBinding{
					                                    .binding         = bindingSlot,
					                                    .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
					                                    .descriptorCount = 1,
					                                    .stageFlags      = vk::ShaderStageFlagBits::eFragment,
					                                  }));
					sizeMap.insert(std::make_pair(bindingSlot, TextureBindingInfo{image.name}));
				}
			}
		}

		if (!fragResources.push_constant_buffers.empty()) { pcShaderStages |= vk::ShaderStageFlagBits::eFragment; }

		MRG_ENGINE_ASSERT(level3UBOBindings.contains(0), "Descriptor set level 3 MUST have a model data uniform at slot 0!")

		std::vector<vk::DescriptorSetLayoutBinding> finalBindings(level2UBOBindings.size() + level2SampledImagesBindings.size());
		std::size_t index = 0;
		for (const auto& ubo : level2UBOBindings) { finalBindings[index++] = ubo.second; }
		for (const auto& sampledImage : level2SampledImagesBindings) { finalBindings[index++] = sampledImage.second; }

		vk::DescriptorSetLayoutCreateInfo setInfo{
		  .bindingCount = static_cast<uint32_t>(finalBindings.size()),
		  .pBindings    = finalBindings.data(),
		};

		level2DSL = m_device.createDescriptorSetLayout(setInfo);

		finalBindings.resize(level3UBOBindings.size() + level3SampledImagesBindings.size());
		index = 0;
		for (const auto& ubo : level3UBOBindings) { finalBindings[index++] = ubo.second; }
		for (const auto& sampledImage : level3SampledImagesBindings) { finalBindings[index++] = sampledImage.second; }

		setInfo = vk::DescriptorSetLayoutCreateInfo{
		  .bindingCount = static_cast<uint32_t>(finalBindings.size()),
		  .pBindings    = finalBindings.data(),
		};

		level3DSL = m_device.createDescriptorSetLayout(setInfo);
	}

	Shader::~Shader()
	{
		m_device.destroyDescriptorSetLayout(level3DSL);
		m_device.destroyDescriptorSetLayout(level2DSL);
		m_device.destroyShaderModule(vertexShaderModule);
		m_device.destroyShaderModule(fragmentShaderModule);
	}

	std::vector<std::uint32_t> Shader::readSource(const char* filePath)
	{
		const auto completePath = Folders::Rendering::shadersFolder + filePath;
		MRG_ENGINE_ASSERT(std::filesystem::exists(completePath), "Shader file \"{}\" does not exists!", completePath)
		std::ifstream file{completePath, std::ios::binary | std::ios::ate};
		const auto fileSize = static_cast<std::size_t>(file.tellg());
		std::vector<std::uint32_t> buffer(fileSize / sizeof(std::uint32_t));
		file.seekg(std::ios::beg);
		file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(fileSize));
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

	Shader::Root Shader::populateUniformData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& uniform)
	{
		Root rootNode{compiler.get_declared_struct_size(compiler.get_type(uniform.type_id))};
		rootNode.name = compiler.get_name(uniform.id);
		rootNode.type = compiler.get_type(uniform.type_id);

		const auto uniformRanges = compiler.get_active_buffer_ranges(uniform.id);
		for (const auto& range : uniformRanges) {
			rootNode.members.emplace_back(getShaderStructData(compiler, uniform.base_type_id, range.index));
		}

		return rootNode;
	}

	Shader::Node
	Shader::getShaderStructData(const spirv_cross::Compiler& compiler, const spirv_cross::TypeID baseType, const uint32_t memberIndex)
	{
		const auto newBaseType = compiler.get_type(baseType).member_types[memberIndex];
		Node currentNode{
		  .name    = compiler.get_member_name(baseType, memberIndex),
		  .type    = compiler.get_type(newBaseType),
		  .members = {},
		};

		for (std::size_t i = 0; i < currentNode.type.member_types.size(); ++i) {
			currentNode.members.emplace_back(getShaderStructData(compiler, newBaseType, static_cast<uint32_t>(i)));
		}

		return currentNode;
	}
}  // namespace MRG
