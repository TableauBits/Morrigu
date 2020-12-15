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
		void resize(uint32_t width, uint32_t height) override;

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
		[[nodiscard]] const std::array<ImVec2, 2>& getUVMapping() const override { return m_UVMapping; }

		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] uint32_t getHandle() { return m_rendererID; }

	private:
		uint32_t m_rendererID = 0;
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 1}, ImVec2{1, 0}};
		uint32_t m_colorAttachment = 0, m_depthAttachment = 0;
	};

}  // namespace MRG::OpenGL

#endif
