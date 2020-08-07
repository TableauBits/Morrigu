#include "VertexArray.h"

#include <glad/glad.h>

namespace
{
	[[nodiscard]] static GLenum ShaderDataTypeToOpenGLBaseType(MRG::ShaderDataType type)
	{
		// clang-format off
		switch (type)
		{
			case MRG::ShaderDataType::Float:  return GL_FLOAT;
			case MRG::ShaderDataType::Float2: return GL_FLOAT;
			case MRG::ShaderDataType::Float3: return GL_FLOAT;
			case MRG::ShaderDataType::Float4: return GL_FLOAT;

			case MRG::ShaderDataType::Mat3:   return GL_FLOAT;
			case MRG::ShaderDataType::Mat4:   return GL_FLOAT;

			case MRG::ShaderDataType::Int:    return GL_INT;
			case MRG::ShaderDataType::Int2:   return GL_INT;
			case MRG::ShaderDataType::Int3:   return GL_INT;
			case MRG::ShaderDataType::Int4:   return GL_INT;

			case MRG::ShaderDataType::Bool:   return GL_BOOL;

			case MRG::ShaderDataType::None:
			default: {
				MRG_CORE_ASSERT(false, fmt::format("Invalid shader data type ! ({})", type));
				return GL_INVALID_ENUM;
			}
		}
		// clang-format on
	}
}  // namespace

namespace MRG::OpenGL
{
	VertexArray::VertexArray() { glCreateVertexArrays(1, &m_rendererID); }
	VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_rendererID); }

	void VertexArray::bind() const { glBindVertexArray(m_rendererID); }
	void VertexArray::unbind() const { glBindVertexArray(0); }

	void VertexArray::addVertexBuffer(const Ref<MRG::VertexBuffer>& vertexBuffer)
	{
		MRG_CORE_ASSERT(vertexBuffer->layout.getElements().size() != 0, "Vertex Buffer layout has not been set !");

		glBindVertexArray(m_rendererID);
		vertexBuffer->bind();

		auto index = 0;
		const auto& layout = vertexBuffer->layout;
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
			                      element.getComponentCount(),
			                      ShaderDataTypeToOpenGLBaseType(element.type),
			                      element.isNormalized ? GL_TRUE : GL_FALSE,
			                      layout.getStride(),
			                      (const void*)(element.offset));
			++index;
		}

		m_vertexBuffers.emplace_back(vertexBuffer);
	}

	void VertexArray::setIndexBuffer(const Ref<MRG::IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_rendererID);
		indexBuffer->bind();

		m_indexBuffer = indexBuffer;
	}
}  // namespace MRG::OpenGL