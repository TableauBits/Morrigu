#include "Framebuffer.h"

namespace
{
	void attachColorTexture(uint32_t id, GLenum format, uint32_t width, uint32_t height, std::size_t index)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<int>(GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_2D, id, 0);
	}

	void attachDepthTexture(uint32_t id, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, id, 0);
	}
}  // namespace

namespace MRG::OpenGL
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	{
		m_specification = spec;
		for (const auto& attachment : spec.attachments.attachments) {
			if (!isDepthFormat(attachment.textureFormat)) {
				m_colorAttachmentsSpecifications.emplace_back(attachment);
			} else {
				m_depthAttachmentsSpecification = attachment;
			}
		}

		m_shader = spec.shaderModule;

		m_isDestroyed = true;
		invalidate();
	}

	Framebuffer::~Framebuffer() { Framebuffer::destroy(); }

	void Framebuffer::destroy()
	{
		if (m_isDestroyed) {
			return;
		}

		m_shader->destroy();

		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(static_cast<int>(m_colorAttachments.size()), m_colorAttachments.data());
		glDeleteTextures(1, &m_depthAttachment);

		m_colorAttachments.clear();
		m_depthAttachment = 0;

		m_isDestroyed = true;
	}

	void Framebuffer::resize(uint32_t width, uint32_t height)
	{
		m_specification.width = width;
		m_specification.height = height;

		invalidate();
	}

	void Framebuffer::invalidate()
	{
		if (!m_isDestroyed) {
            glDeleteFramebuffers(1, &m_rendererID);
            glDeleteTextures(static_cast<int>(m_colorAttachments.size()), m_colorAttachments.data());
            glDeleteTextures(1, &m_depthAttachment);

            m_colorAttachments.clear();
            m_depthAttachment = 0;
        }

		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		if (!m_colorAttachmentsSpecifications.empty()) {
			m_colorAttachments.resize(m_colorAttachmentsSpecifications.size());
			glCreateTextures(GL_TEXTURE_2D, static_cast<int>(m_colorAttachments.size()), m_colorAttachments.data());

			for (std::size_t i = 0; i < m_colorAttachments.size(); ++i) {
				glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i]);

				switch (m_colorAttachmentsSpecifications[i].textureFormat) {
				case FramebufferTextureFormat::RGBA8: {
					attachColorTexture(m_colorAttachments[i], GL_RGBA8, m_specification.width, m_specification.height, i);
				} break;
				case FramebufferTextureFormat::RGBA16: {
					attachColorTexture(m_colorAttachments[i], GL_RGBA16, m_specification.width, m_specification.height, i);
				} break;

				case FramebufferTextureFormat::DEPTH24STENCIL8:
				case FramebufferTextureFormat::None: {
				} break;
				}
			}
		}

		if (m_depthAttachmentsSpecification.textureFormat != FramebufferTextureFormat::None) {
			glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
			switch (m_depthAttachmentsSpecification.textureFormat) {
			case FramebufferTextureFormat::DEPTH24STENCIL8: {
				attachDepthTexture(
				  m_depthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_specification.width, m_specification.height);
			} break;

			case FramebufferTextureFormat::RGBA8:
			case FramebufferTextureFormat::RGBA16:
			case FramebufferTextureFormat::None: {
			} break;
			}
		}

		if (m_colorAttachments.size() > 1) {
			MRG_CORE_ASSERT(m_colorAttachments.size() <= 4, "Too many color attachments!")
			std::array<GLenum, 4> buffers = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
			glDrawBuffers(static_cast<int>(m_colorAttachments.size()), buffers.data());
		} else if (m_colorAttachments.empty()) {
			glDrawBuffer(GL_NONE);
		}

		MRG_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_isDestroyed = false;
	}

	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specification.width, m_specification.height);
	}
	void Framebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}  // namespace MRG::OpenGL
