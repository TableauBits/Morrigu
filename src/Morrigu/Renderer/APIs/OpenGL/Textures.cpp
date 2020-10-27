#include "Textures.h"

#include "Debug/Instrumentor.h"
#include "Renderer/ImageLoader.h"

#include <stb_image.h>

namespace MRG::OpenGL
{
	Texture2D::Texture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height)
	{
		MRG_PROFILE_FUNCTION();

		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture2D::Texture2D(const std::string& path)
	{
		MRG_PROFILE_FUNCTION();

		int width, height, channels;
		const auto data = ImageLoader::loadFromFile(path.c_str(), &width, &height, &channels, STBI_rgb_alpha, true);
		MRG_CORE_ASSERT(data, fmt::format("Failed to load file '{}'", path));

		m_width = width;
		m_height = height;

		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		MRG_CORE_ASSERT(m_internalFormat & m_dataFormat, "File format not supported !");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void Texture2D::destroy()
	{
		if (m_isDestroyed)
			return;

		glDeleteTextures(1, &m_rendererID);
		m_isDestroyed = true;
	}

	void Texture2D::setData(void* data, [[maybe_unused]] uint32_t size)
	{
		MRG_PROFILE_FUNCTION();

		[[maybe_unused]] const auto bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		MRG_CORE_ASSERT(size == m_width * m_width * bpp, "Data size is incorrect !");

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::bind(uint32_t slot) const
	{
		MRG_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_rendererID);
	}
}  // namespace MRG::OpenGL