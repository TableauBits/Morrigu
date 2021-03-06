#ifndef MRG_OPENGL_IMPL_FRAMEBUFFER
#define MRG_OPENGL_IMPL_FRAMEBUFFER

#include "Renderer/APIs/OpenGL/Renderer2D.h"
#include "Renderer/Framebuffer.h"

namespace MRG::OpenGL
{
	class Framebuffer : public MRG::Framebuffer
	{
	public:
		explicit Framebuffer(const FramebufferSpecification& spec);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = delete;
		~Framebuffer() override;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = delete;

		void destroy() override;
		void resize(uint32_t width, uint32_t height) override;

		void invalidate();

		void bind();
		static void unbind();

		[[nodiscard]] virtual ImTextureID getImTextureID(uint32_t index = 0) override
		{
			return (ImTextureID)(uintptr_t)m_colorAttachments[index];
		}
		[[nodiscard]] const std::array<ImVec2, 2>& getUVMapping() const override { return m_UVMapping; }

		[[nodiscard]] virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }
		[[nodiscard]] uint32_t getHandle() { return m_rendererID; }
		[[nodiscard]] Ref<Shader> getShader() { return m_shader; }

	private:
		uint32_t m_rendererID = 0;
		std::array<ImVec2, 2> m_UVMapping = {ImVec2{0, 1}, ImVec2{1, 0}};

		Ref<Shader> m_shader;
		std::vector<uint32_t> m_colorAttachments{};
		uint32_t m_depthAttachment = 0;
	};

}  // namespace MRG::OpenGL

#endif
