//
// Created by Mathis Lamidey on 2021-05-08.
//

#ifndef MORRIGU_MESH_H
#define MORRIGU_MESH_H

#include "Rendering/GLMIncludeHelper.h"
#include "Rendering/VkTypes.h"

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

	class Mesh
	{
	public:
		struct PushConstants
		{
			glm::vec4 data;
			glm::mat4 transform;
		};

		std::vector<Vertex> vertices;
		AllocatedBuffer vertexBuffer;

		/// Filepath is already pointing to the meshes folder, so you can omit the path to that folder yourself
		static Ref<Mesh> loadFromFile(const char* filePath);
	};
}  // namespace MRG

#endif  // MORRIGU_MESH_H
