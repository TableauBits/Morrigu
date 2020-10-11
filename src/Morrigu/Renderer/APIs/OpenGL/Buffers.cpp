#include "Buffers.h"

#include "Debug/Instrumentor.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
	{
		MRG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void VertexBuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		glDeleteBuffers(1, &m_rendererID);
		m_isDestroyed = true;
	}

	void VertexBuffer::bind() const
	{
		MRG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}
	void VertexBuffer::unbind() const
	{
		MRG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//===================================================================================//

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) : m_count(count)
	{
		MRG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void IndexBuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		glDeleteBuffers(1, &m_rendererID);
		m_isDestroyed = true;
	}

	void IndexBuffer::bind() const
	{
		MRG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}
	void IndexBuffer::unbind() const
	{
		MRG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}  // namespace MRG::OpenGL
