#ifndef MRG_CLASSES_BUFFERS
#define MRG_CLASSES_BUFFERS

#include "Core/Core.h"

#include <cstdint>

namespace MRG
{
	// clang-format off
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	[[nodiscard]] static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:  return 4 * 1;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;

		case ShaderDataType::Mat3:   return 4 * 3 * 3;
		case ShaderDataType::Mat4:   return 4 * 4 * 4;

		case ShaderDataType::Int:    return 4 * 1;
		case ShaderDataType::Int2:   return 4 * 2;
		case ShaderDataType::Int3:   return 4 * 3;
		case ShaderDataType::Int4:   return 4 * 4;

		case ShaderDataType::Bool:   return 1;

		case ShaderDataType::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("Invalid shader data type ! ({})", type));
			return 0;
		}
		}

	}
	// clang-format on

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint64_t offset;
		bool isNormalized;

		BufferElement() = default;
		BufferElement(ShaderDataType bufferType, const std::string& bufferName, bool normalized = false)
		    : name(bufferName), type(bufferType), size(ShaderDataTypeSize(bufferType)), offset(0), isNormalized(normalized)
		{}

		[[nodiscard]] uint32_t getComponentCount() const
		{
			// clang-format off
			switch (type)
			{
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;

			case ShaderDataType::Mat3:   return 3 * 3;
			case ShaderDataType::Mat4:   return 4 * 4;

			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;

			case ShaderDataType::Bool:   return 1;

			case ShaderDataType::None:
			default: {
				MRG_CORE_ASSERT(false, fmt::format("Invalid shader data type ! ({})", type));
				return 0;
			}
			}
			// clang-format on
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements)
		{
			uint64_t offset = 0;
			for (auto& element : m_elements) {
				element.offset = offset;
				m_stride += element.size;
				offset += element.size;
			}
		}

		[[nodiscard]] inline auto getStride() const { return m_stride; }
		[[nodiscard]] inline auto getElements() const { return m_elements; }

		[[nodiscard]] inline auto begin() const { return m_elements.begin(); }
		[[nodiscard]] inline auto end() const { return m_elements.end(); }

	private:
		std::vector<BufferElement> m_elements;
		uint32_t m_stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		BufferLayout layout;

		[[nodiscard]] static Ref<VertexBuffer> create(const float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		[[nodiscard]] virtual uint32_t getCount() const = 0;

		[[nodiscard]] static Ref<IndexBuffer> create(const uint32_t* indices, uint32_t size);
	};
}  // namespace MRG

#endif