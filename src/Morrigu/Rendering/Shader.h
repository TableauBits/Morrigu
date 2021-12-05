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
		struct TextureBindingInfo
		{
			std::string name;
		};

		struct Node
		{
			std::string name{};
			spirv_cross::SPIRType type;
			std::vector<Node> members;
		};

		struct Root : public Node
		{
			Root(std::size_t treeSize) : Node{"root", {}, {}}, size{treeSize} {};
			std::size_t size{};
		};

		explicit Shader(vk::Device device, const char* vertexShaderName, const char* fragmentShaderName);
		~Shader();

		vk::ShaderModule vertexShaderModule;
		vk::ShaderModule fragmentShaderModule;

		vk::DescriptorSetLayout level2DSL;
		vk::DescriptorSetLayout level3DSL;

		// level 2 bindings
		std::map<uint32_t, Root> l2UBOData;
		std::map<uint32_t, TextureBindingInfo> l2ImageBindings;
		// level 3 bindings
		std::map<uint32_t, Root> l3UBOData;
		std::map<uint32_t, TextureBindingInfo> l3ImageBindings;

	private:
		[[nodiscard]] static std::vector<std::uint32_t> readSource(const char* filePath);
		[[nodiscard]] vk::ShaderModule loadShaderModule(const std::vector<uint32_t>& src);
		[[nodiscard]] static Root populateUniformData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& uniform);
		[[nodiscard]] static Node
		getShaderStructData(const spirv_cross::Compiler& compiler, spirv_cross::TypeID baseType, uint32_t memberIndex);

		vk::Device m_device;
	};
}  // namespace MRG

#endif  // MORRIGU_SHADER_H
