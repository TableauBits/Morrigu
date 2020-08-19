#ifndef MRG_OPENGL_IMPL_TEXTURES
#define MRG_OPENGL_IMPL_TEXTURES

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

		[[nodiscard]] uint32_t getWidth() const override { return m_width; };
		[[nodiscard]] uint32_t getHeight() const override { return m_height; };

		void setData(void* data, uint32_t size) override;

		void bind(uint32_t slot = 0) const override;

	private:
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};
}  // namespace MRG::OpenGL

#endif