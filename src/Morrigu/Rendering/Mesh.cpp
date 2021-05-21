//
// Created by Mathis Lamidey on 2021-05-08.
//

#include "Mesh.h"

#include "Core/FileNames.h"

#include <tiny_obj_loader.h>

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

	Ref<Mesh> Mesh::loadFromFile(const char* filePath)
	{
		auto newMesh = createRef<Mesh>();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string errors;

		const auto result = tinyobj::LoadObj(&attrib, &shapes, &materials, &errors, (Folders::Rendering::meshesFolder + filePath).c_str());
		if (!result) { MRG_ENGINE_WARN("Errors encountered when loading mesh:\n\"{}\"", errors) }

		for (const auto& shape : shapes) {
			std::size_t indexOffset = 0;
			for ([[maybe_unused]] const auto& face : shape.mesh.num_face_vertices) {
				static const int shapeVertexCount = 3;  // Only triangles for now
				for (int vertexIndex = 0; vertexIndex < shapeVertexCount; ++vertexIndex) {
					const auto idx = shape.mesh.indices[indexOffset + vertexIndex];

					// position
					const auto posX = attrib.vertices[3 * idx.vertex_index + 0];
					const auto posY = attrib.vertices[3 * idx.vertex_index + 1];
					const auto posZ = attrib.vertices[3 * idx.vertex_index + 2];

					// normals
					const auto normX = attrib.normals[3 * idx.normal_index + 0];
					const auto normY = attrib.normals[3 * idx.normal_index + 1];
					const auto normZ = attrib.normals[3 * idx.normal_index + 2];

					newMesh->vertices.emplace_back(Vertex{
					  .position = {posX, posY, posZ},
					  .normal   = {normX, normY, normZ},
					  .color    = {normX, normY, normZ},  // Set the color as the normal for now
					});
				}
				indexOffset += shapeVertexCount;
			}
		}

		return newMesh;
	}
}  // namespace MRG
