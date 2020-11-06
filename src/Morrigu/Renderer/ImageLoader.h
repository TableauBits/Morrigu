#ifndef MRG_HELPER_IMAGELOADER
#define MRG_HELPER_IMAGELOADER

namespace MRG::ImageLoader
{
	unsigned char* loadFromFile(const char* filename, int* x, int* y, int* comp, int req_comp, bool flipped);
}

#endif