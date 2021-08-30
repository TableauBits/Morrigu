//
// Created by Mathis Lamidey on 2021-07-24.
//

#include "Text.h"

#include "Rendering/Renderer.h"
#include "Utils/Meshes.h"

namespace MRG::UI
{
	Text::Text(const std::string& content, const Ref<Font>& font, const glm::vec2& position, Renderer* renderer)
	{
		m_letters.reserve(content.size());

		float posX       = position.x;
		float posY       = position.y;
		const auto glyph = font->face->glyph;
		for (auto letter : content) {
			const auto error = FT_Load_Char(font->face, letter, FT_LOAD_RENDER);
			if (error) { continue; }

			std::vector<std::byte> fullBitmapData{glyph->bitmap.width * glyph->bitmap.rows * 4};
			auto* readHead = glyph->bitmap.buffer;
			auto writeHead = fullBitmapData.begin();

			for (auto pixelCount = 0u; pixelCount < glyph->bitmap.width * glyph->bitmap.rows; ++pixelCount) {
				*writeHead++ = std::byte{0x00};
				*writeHead++ = std::byte{0x00};
				*writeHead++ = std::byte{0x00};
				*writeHead++ = std::byte{*readHead++};
			}

			const auto xAbs = glyph->metrics.width >> 6;
			const auto yAbs = glyph->metrics.height >> 6;
			auto quad       = createRef<Mesh<TexturedVertex>>();
			quad->vertices  = {
              MRG::TexturedVertex{.position{0.f, -yAbs, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 1.f}},
              MRG::TexturedVertex{.position{0.f, 0.f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 0.f}},
              MRG::TexturedVertex{.position{xAbs, 0.f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 0.f}},
              MRG::TexturedVertex{.position{xAbs, 0.f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 0.f}},
              MRG::TexturedVertex{.position{xAbs, -yAbs, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 1.f}},
              MRG::TexturedVertex{.position{0.f, -yAbs, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 1.f}},
            };
			const auto bitmap       = renderer->createTexture(fullBitmapData.data(), glyph->bitmap.width, glyph->bitmap.rows);
			const auto renderLetter = renderer->createRenderObject(quad, renderer->textUIMaterial);
			renderLetter->bindTexture(1, bitmap);
			renderLetter->translate({posX, posY - (glyph->metrics.horiBearingY >> 6), 0.f});
			renderer->uploadMesh(renderLetter->mesh);
			m_letters.emplace_back(Letter{bitmap, renderLetter});

			posX += glyph->advance.x >> 6;
			posY += glyph->advance.y >> 6;
		}
	}

	std::vector<RenderData> Text::getCollectionRenderData()
	{
		std::vector<RenderData> rd;
		rd.reserve(m_letters.size());
		for (const auto& letter : m_letters) { rd.emplace_back(letter.quad->getRenderData()); }

		return rd;
	}
}  // namespace MRG::UI
