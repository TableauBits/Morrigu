//
// Created by Mathis Lamidey on 2021-06-11.
//

#include "Meshes.h"

#include <tiny_obj_loader.h>

namespace MRG::Utils::Meshes
{
	template<>
	Ref<Mesh<BasicVertex>> loadMeshFromFile(const char* filePath)
	{
		auto newMesh = createRef<Mesh<BasicVertex>>();

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

					newMesh->vertices.emplace_back(BasicVertex{
					  .position = {posX, posY, posZ},
					  .normal   = {normX, normY, normZ},
					});
				}
				indexOffset += shapeVertexCount;
			}
		}

		return newMesh;
	}

	template<>
	Ref<Mesh<ColoredVertex>> loadMeshFromFile(const char* filePath)
	{
		auto newMesh = createRef<Mesh<ColoredVertex>>();

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

					newMesh->vertices.emplace_back(ColoredVertex{
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

	template<>
	Ref<Mesh<TexturedVertex>> loadMeshFromFile(const char* filePath)
	{
		auto newMesh = createRef<Mesh<TexturedVertex>>();

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

					// texture coordinates
					const auto texCoodX = attrib.texcoords[2 * idx.texcoord_index + 0];
					const auto texCoodY = attrib.texcoords[2 * idx.texcoord_index + 1];

					newMesh->vertices.emplace_back(TexturedVertex{
					  .position  = {posX, posY, posZ},
					  .normal    = {normX, normY, normZ},
					  .texCoords = {texCoodX, 1 - texCoodY},
					});
				}
				indexOffset += shapeVertexCount;
			}
		}

		return newMesh;
	}
	template<>
	Ref<Mesh<BasicVertex>> quad()
	{
		auto quad      = createRef<Mesh<BasicVertex>>();
		quad->vertices = {
		  BasicVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		  BasicVertex{.position{-0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		  BasicVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		  BasicVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		  BasicVertex{.position{0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		  BasicVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}},
		};

		return quad;
	}

	template<>
	Ref<Mesh<ColoredVertex>> quad()
	{
		auto quad      = createRef<Mesh<ColoredVertex>>();
		quad->vertices = {
		  ColoredVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		  ColoredVertex{.position{-0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		  ColoredVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		  ColoredVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		  ColoredVertex{.position{0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		  ColoredVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .color{0.2, 0.2, 0.2}},
		};

		return quad;
	}

	template<>
	Ref<Mesh<TexturedVertex>> quad()
	{
		auto quad      = MRG::createRef<MRG::Mesh<MRG::TexturedVertex>>();
		quad->vertices = {
		  MRG::TexturedVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 0.f}},
		  MRG::TexturedVertex{.position{-0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 1.f}},
		  MRG::TexturedVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 1.f}},
		  MRG::TexturedVertex{.position{0.5f, 0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 1.f}},
		  MRG::TexturedVertex{.position{0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{1.f, 0.f}},
		  MRG::TexturedVertex{.position{-0.5f, -0.5f, 0.f}, .normal{0.f, 0.f, -1.f}, .texCoords{0.f, 0.f}},
		};

		return quad;
	}
}  // namespace MRG::Utils::Meshes
