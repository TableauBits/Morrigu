//
// Created by Mathis Lamidey on 2021-05-08.
//

#ifndef MORRIGU_MESH_H
#define MORRIGU_MESH_H

#include "Core/FileNames.h"
#include "Rendering/GLMIncludeHelper.h"
#include "Rendering/Vertex.h"
#include "Rendering/VkTypes.h"

#include <vector>

namespace MRG
{
	struct MeshPushConstants
	{
		glm::mat4 modelMatrix;
	};
	template<Vertex VertexType>
	class Mesh
	{
	public:
		std::vector<VertexType> vertices;
		AllocatedBuffer vertexBuffer;
	};
}  // namespace MRG

#endif  // MORRIGU_MESH_H
