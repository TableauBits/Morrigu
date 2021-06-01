//
// Created by Mathis Lamidey on 2021-05-29.
//

#ifndef MORRIGU_VERTEX_H
#define MORRIGU_VERTEX_H

#include "Rendering/VkTypes.h"
#include "Utils/GLMIncludeHelper.h"

#include <vector>

namespace MRG
{
	struct VertexInputDescription
	{
		std::vector<vk::VertexInputBindingDescription> bindings{};
		std::vector<vk::VertexInputAttributeDescription> attributes{};
	};

	// clang-format off
    template<typename VertexType>
    concept Vertex = requires(VertexType vertex) {
        {VertexType::getVertexDescription()} -> std::same_as<VertexInputDescription>;
    };
	// clang-format on

	// These are sample implementations of some basic vertex types
	struct BasicVertex
	{
		glm::vec3 position{};
		glm::vec3 normal{};

		static VertexInputDescription getVertexDescription();
	};

	struct ColoredVertex
	{
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec3 color{};

		static VertexInputDescription getVertexDescription();
	};

	struct TexturedVertex
	{
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 texCoords{};

		static VertexInputDescription getVertexDescription();
	};
}  // namespace MRG

#endif  // MORRIGU_VERTEX_H
