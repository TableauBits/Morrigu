#include "VertexArray.h"

#include "Debug/Instrumentor.h"

namespace
{
	[[nodiscard]] static VkFormat ShaderDataTypeToVulkanBaseType(MRG::ShaderDataType type)
	{
		// clang-format off
		switch (type)
		{
			case MRG::ShaderDataType::Float:  return VkFormat::VK_FORMAT_R32_SFLOAT;
			case MRG::ShaderDataType::Float2: return VkFormat::VK_FORMAT_R32G32_SFLOAT;
			case MRG::ShaderDataType::Float3: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			case MRG::ShaderDataType::Float4: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

			// TODO: Do something special for matrices maybe ?
			case MRG::ShaderDataType::Mat3:   return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			case MRG::ShaderDataType::Mat4:   return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

			case MRG::ShaderDataType::Int:    return VkFormat::VK_FORMAT_R32_SINT;
			case MRG::ShaderDataType::Int2:   return VkFormat::VK_FORMAT_R32G32_SINT;
			case MRG::ShaderDataType::Int3:   return VkFormat::VK_FORMAT_R32G32B32_SINT;
			case MRG::ShaderDataType::Int4:   return VkFormat::VK_FORMAT_R32G32B32A32_SINT;

			// TODO: Get a better look at this
			case MRG::ShaderDataType::Bool:   return VkFormat::VK_FORMAT_R8_SINT;

			case MRG::ShaderDataType::None:
			default: {
				MRG_CORE_ASSERT(false, fmt::format("Invalid shader data type ! ({})", type));
				return VkFormat::VK_FORMAT_UNDEFINED;
			}
		}
		// clang-format on
	}
}  // namespace

namespace MRG::Vulkan
{
	VertexArray::VertexArray()
	{
		MRG_PROFILE_FUNCTION();

		// Vulkan doesn't have actual vertex array objects

		// That may be a good place to some optimized memory allocation
		// Having a VkDeviceMemory shared for both the vertex and index buffer

		// TODO ?
	}
	VertexArray::~VertexArray()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void VertexArray::bind() const
	{
		// MRG_PROFILE_FUNCTION();
	}
	void VertexArray::unbind() const
	{
		// MRG_PROFILE_FUNCTION();
	}

	void VertexArray::addVertexBuffer(const Ref<MRG::VertexBuffer>& vertexBuffer)
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(m_vertexBuffers.size() == 0, "Only a single vertex buffer per vertex array is supported right now!");
		MRG_CORE_ASSERT(vertexBuffer->layout.getElements().size() != 0, "Vertex Buffer layout has not been set!");

		m_bindingDescription.binding = 0;
		m_bindingDescription.stride = vertexBuffer->layout.getStride();
		m_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		uint32_t location = 0;
		for (const auto& element : vertexBuffer->layout) {
			VkVertexInputAttributeDescription attribDesc{};
			attribDesc.binding = 0;
			attribDesc.location = location;
			attribDesc.format = ShaderDataTypeToVulkanBaseType(element.type);
			attribDesc.offset = static_cast<uint32_t>(element.offset);

			m_attributeDescriptions.emplace_back(attribDesc);

			// TODO: handle location jumps superior to 1
			++location;
		}

		m_vertexBuffers.emplace_back(vertexBuffer);
	}

	void VertexArray::setIndexBuffer(const Ref<MRG::IndexBuffer>& indexBuffer)
	{
		MRG_PROFILE_FUNCTION();

		m_indexBuffer = indexBuffer;
	}
}  // namespace MRG::Vulkan