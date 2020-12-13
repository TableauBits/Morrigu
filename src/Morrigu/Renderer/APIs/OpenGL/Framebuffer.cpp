#include "Framebuffer.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	{
		m_specification = spec;
		invalidate();
	}

	Framebuffer::~Framebuffer() { destroy(); }

	void Framebuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		glDeleteFramebuffers(1, &m_rendererID);

		m_isDestroyed = true;
	}

	void Framebuffer::invalidate()
	{
		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specification.width, m_specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_specification.width, m_specification.height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		MRG_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID); }
	void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
}  // namespace MRG::OpenGL
