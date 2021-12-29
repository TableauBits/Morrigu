//
// Created by Mathis Lamidey on 2021-06-11.
//

#ifndef MORRIGU_UTILSMESHES_H
#define MORRIGU_UTILSMESHES_H

#include "Rendering/Mesh.h"

namespace MRG::Utils::Meshes
{
	/// use template specialization to implement this function to your custom vertex type.
	/// To be interchangible with other implementations,
	/// these functions should prefix the given path with MRG::Folders::Rendering::meshesFolder
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> loadMeshFromFile(const char* filePath);

	template<Vertex VertexType>
	Ref<Mesh<VertexType>> quad();
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> disk()
	{
		return loadMeshFromFile<VertexType>("engine/disk.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> cube()
	{
		return loadMeshFromFile<VertexType>("engine/cube.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> sphere()
	{
		return loadMeshFromFile<VertexType>("engine/sphere.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> cylinder()
	{
		return loadMeshFromFile<VertexType>("engine/cylinder.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> torus()
	{
		return loadMeshFromFile<VertexType>("engine/torus.obj");
	}
}  // namespace MRG::Utils::Meshes

#endif  // MORRIGU_UTILSMESHES_H
