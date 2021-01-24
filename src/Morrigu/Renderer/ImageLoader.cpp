#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace MRG::ImageLoader
{
	unsigned char* loadFromFile(const char* filename, int* x, int* y, int* comp, int req_comp, bool flipped)
	{
		stbi_set_flip_vertically_on_load(static_cast<int>(flipped));
		return stbi_load(filename, x, y, comp, req_comp);
	}
}  // namespace MRG::ImageLoader
