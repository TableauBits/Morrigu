//
// Created by Mathis Lamidey on 2021-07-24.
//

#include "Font.h"

#include "Core/Core.h"
#include "Core/FileNames.h"

#include <GLFW/glfw3.h>

namespace MRG::UI
{
	Font::Font(const std::string& file, FT_Library& ftHandle)
	{
		const auto fontPath = Folders::Rendering::fontsFolder + file;
		const auto error    = FT_New_Face(ftHandle, fontPath.c_str(), 0, &face);
		MRG_ENGINE_ASSERT(!error, "Unable to load font \"{}\"!", fontPath)

		setSize(16);

		// @TODO(Ithyx): pre-generate/cache atlas ?
	}

	Font& Font::operator=(Font&& font) noexcept
	{
		face      = font.face;
		font.face = nullptr;
		return *this;
	}

	Font::~Font()
	{
		if (face != nullptr) { FT_Done_Face(face); }
	}

	void Font::setSize(uint32_t pointSize) { FT_Set_Char_Size(face, 0, pointSize * 64, 96, 96); }
}  // namespace MRG::UI
