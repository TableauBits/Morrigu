//
// Created by Mathis Lamidey on 2021-07-24.
//

#include "Font.h"

#include "Core/Core.h"
#include "Core/FileNames.h"

namespace MRG::UI
{
	Font::Font(const std::string& file, FT_Library& ftHandle)
	{
		const auto fontPath = Folders::Rendering::fontsFolder + file;
		const auto error    = FT_New_Face(ftHandle, fontPath.c_str(), 0, &face);
		MRG_ENGINE_ASSERT(!error, "Unable to load font \"{}\"!", fontPath)

		FT_Set_Char_Size(face, 0, 50 * 64, 92, 92);

		// @TODO(Ithyx): pre-generate/cache atlas ?
	}

	Font& Font::operator=(Font&& font) noexcept
	{
		std::swap(face, font.face);
		return *this;
	}

	Font::~Font()
	{
		if (face != nullptr) { FT_Done_Face(face); }
	}
}  // namespace MRG::UI
