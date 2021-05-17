//
// Created by Mathis Lamidey on 2021-05-08.
//

#include "Mesh.h"
namespace MRG
{
	VertexInputDescription Vertex::getVertexDescription()
	{
		vk::VertexInputBindingDescription mainBinding{
		  .binding   = 0,
		  .stride    = sizeof(Vertex),
		  .inputRate = vk::VertexInputRate::eVertex,
		};

		vk::VertexInputAttributeDescription position{
		  .location = 0,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(Vertex, position)),
		};
		vk::VertexInputAttributeDescription normal{
		  .location = 1,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(Vertex, normal)),
		};
		vk::VertexInputAttributeDescription color{
		  .location = 2,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(Vertex, color)),
		};

		return VertexInputDescription{.bindings{mainBinding}, .attributes{position, normal, color}};
	}

	void Mesh::rotate(const glm::vec3& axis, float radRotation) { modelMatrix = glm::rotate(modelMatrix, radRotation, axis); }
	void Mesh::scale(const glm::vec3& scaling) { modelMatrix = glm::scale(modelMatrix, scaling); }
	void Mesh::translate(const glm::vec3& translation) { modelMatrix = glm::translate(modelMatrix, translation); }
}  // namespace MRG
