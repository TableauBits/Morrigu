#ifndef MRG_OPENGL_IMPL_TEXTURES
#define MRG_OPENGL_IMPL_TEXTURES

#include "Core/Warnings.h"
#include "Renderer/Textures.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	class Texture2D : public MRG::Texture2D
	{
	public:
		Texture2D(uint32_t width, uint32_t height);
		Texture2D(const std::string& path);
		virtual ~Texture2D();

		bool operator==(const Texture& other) const override { return m_rendererID == ((OpenGL::Texture2D&)other).m_rendererID; }

		void destroy() override;

		[[nodiscard]] uint32_t getWidth() const override { return m_width; };
		[[nodiscard]] uint32_t getHeight() const override { return m_height; };
		[[nodiscard]] uint32_t getHandle() const { return m_rendererID; }
		[[nodiscard]] ImTextureID getImTextureID() override { return (ImTextureID)(uintptr_t)m_rendererID; };

		void setData(void* data, uint32_t size) override;

		void bind(uint32_t slot = 0) const override;

	private:
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};
}  // namespace MRG::OpenGL

#endif