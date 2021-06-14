//
// Created by Mathis Lamidey on 2021-05-28.
//

#ifndef MORRIGU_SHADER_H
#define MORRIGU_SHADER_H

#include "Rendering/RendererTypes.h"

#include <spirv_cross/spirv_reflect.hpp>

#include <map>

namespace MRG
{
	class Shader
	{
	public:
		explicit Shader(vk::Device device, const char* vertexShaderName, const char* fragmentShaderName, DeletionQueue& deletionQueue);

		vk::ShaderModule vertexShaderModule;
		vk::ShaderModule fragmentShaderModule;

		vk::DescriptorSetLayout level2DSL;
		vk::DescriptorSetLayout level3DSL;
		std::vector<vk::PushConstantRange> pcRanges;

		// level 2 bindings
		std::map<uint32_t, std::size_t> l2UBOSizes;
		std::vector<uint32_t> l2ImageBindings;
		// level 3 bindings
		std::map<uint32_t, std::size_t> l3UBOSizes;
		std::vector<uint32_t> l3ImageBindings;

	private:
		[[nodiscard]] static std::vector<std::uint32_t> readSource(const char* filePath);
		[[nodiscard]] vk::ShaderModule loadShaderModule(const std::vector<uint32_t>& src);

		vk::Device m_device;
	};
}  // namespace MRG

#endif  // MORRIGU_SHADER_H
