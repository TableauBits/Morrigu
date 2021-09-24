//
// Created by Mathis Lamidey on 2021-07-24.
//

#ifndef MORRIGU_TEXT_H
#define MORRIGU_TEXT_H

#include "Core/Core.h"
#include "Rendering/RenderObject.h"
#include "Rendering/UI/Font.h"

namespace MRG
{
	class Renderer;
}

namespace MRG::UI
{
	class Text
	{
	public:
		Text(const std::string& content, const Ref<Font>& font, const glm::vec2& position, Renderer* renderer);

		[[nodiscard]] std::vector<RenderData> getCollectionRenderData();

	private:
		struct Letter
		{
			Ref<Texture> bitmap;
			Ref<RenderObject<TexturedVertex>> quad;
		};
		std::vector<Letter> m_letters;
	};
}  // namespace MRG::UI

#endif  // MORRIGU_TEXT_H