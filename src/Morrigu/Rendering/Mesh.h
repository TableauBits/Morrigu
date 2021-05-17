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
		glm::mat4 modelMatrix{1.f};

		void rotate(const glm::vec3& axis, float radRotation);
		void scale(const glm::vec3& scaling);
		void translate(const glm::vec3& translation);
	};
}  // namespace MRG

#endif  // MORRIGU_MESH_H
