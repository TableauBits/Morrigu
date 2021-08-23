//
// Created by Mathis Lamidey on 2021-07-24.
//

#ifndef MORRIGU_FONT_H
#define MORRIGU_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace MRG::UI
{
	class Font
	{
	public:
		Font(const std::string& file, FT_Library& ftHandle);
		Font(const Font& font) = delete;
		Font& operator=(const Font& font) = delete;

		Font(Font&& font) noexcept : face(std::exchange(font.face, nullptr)) {}
		Font& operator=(Font&& font) noexcept;

		~Font();

		FT_Face face;
	};
}  // namespace MRG::UI

#endif  // MORRIGU_FONT_H
