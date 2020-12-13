#ifndef MRG_OPENGL_IMPL_FRAMEBUFFER
#define MRG_OPENGL_IMPL_FRAMEBUFFER

#include "Core/Warnings.h"
#include "Renderer/APIs/OpenGL/Renderer2D.h"
#include "Renderer/Framebuffer.h"

namespace MRG::OpenGL
{
	class Framebuffer : public MRG::Framebuffer
	{
	public:
		Framebuffer(const FramebufferSpecification& spec);
		virtual ~Framebuffer();

		void destroy() override;

		void invalidate();

		void bind();
		void unbind();

		[[nodiscard]] virtual ImTextureID getImTextureID() override
		{
			DISABLE_WARNING_PUSH;
			DISABLE_WARNING_GREATER_SIZE_CAST;
			return (ImTextureID)m_colorAttachment;
			DISABLE_WARNING_POP;
		}

		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] uint32_t getHandle() { return m_rendererID; }

	private:
		uint32_t m_rendererID;
		uint32_t m_colorAttachment, m_depthAttachment;
	};

}  // namespace MRG::OpenGL

#endif
