#include "Textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>

namespace MRG::OpenGL
{
	Texture2D::Texture2D(const std::string& path) : m_path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		const auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		MRG_CORE_ASSERT(data, fmt::format("Failed to load file '{}'", path));

		m_width = width;
		m_height = height;

		GLenum internalFormat = channels == 4 ? GL_RGBA8 : (channels == 3 ? GL_RGB8 : 0);
		GLenum dataFormat = channels == 4 ? GL_RGBA : (channels == 3 ? GL_RGB : 0);

		MRG_CORE_ASSERT(internalFormat & dataFormat, "File format not supported !");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D() { glDeleteTextures(1, &m_rendererID); }

	void Texture2D::bind(uint32_t slot) const { glBindTextureUnit(slot, m_rendererID); }
}  // namespace MRG::OpenGL