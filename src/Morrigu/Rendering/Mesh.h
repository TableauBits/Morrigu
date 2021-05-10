//
// Created by Mathis Lamidey on 2021-05-08.
//

#ifndef MORRIGU_MESH_H
#define MORRIGU_MESH_H

#include "Rendering/VkTypes.h"

#include <glm/vec3.hpp>

#include <vector>

namespace MRG
{
	struct VertexInputDescription
	{
		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;

		static VertexInputDescription getVertexDescription();
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		AllocatedBuffer vertexBuffer;
	};
}  // namespace MRG

#endif  // MORRIGU_MESH_H
