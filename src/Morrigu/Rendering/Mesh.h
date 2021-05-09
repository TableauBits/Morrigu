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
	struct Vertex {
        glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		AllocatedBuffer vertexBuffer;
	};
}  // namespace MRG

#endif  // MORRIGU_MESH_H
