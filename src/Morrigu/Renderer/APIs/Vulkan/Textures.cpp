#include "Textures.h"

#include "Debug/Instrumentor.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

namespace MRG::Vulkan
{
	Texture2D::Texture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height)
	{
		// MRG_PROFILE_FUNCTION();

		m_internalFormat = 0;
		m_dataFormat = 0;
	}

	Texture2D::Texture2D(const std::string&)
	{
		// MRG_PROFILE_FUNCTION();

		// int width, height, channels;
		// stbi_set_flip_vertically_on_load(1);
		// const auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		// MRG_CORE_ASSERT(data, fmt::format("Failed to load file '{}'", path));

		m_width = 0;
		m_height = 0;

		m_internalFormat = 0;
		m_dataFormat = 0;

		// stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void Texture2D::destroy() {}

	void Texture2D::setData(void*, uint32_t)
	{
		// MRG_PROFILE_FUNCTION();
	}

	void Texture2D::bind(uint32_t) const
	{
		// MRG_PROFILE_FUNCTION();
	}
}  // namespace MRG::Vulkan