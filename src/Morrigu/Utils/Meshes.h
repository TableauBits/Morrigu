//
// Created by Mathis Lamidey on 2021-06-11.
//

#ifndef MORRIGU_UTILSMESHES_H
#define MORRIGU_UTILSMESHES_H

#include "Rendering/Mesh.h"

namespace MRG::Utils::Meshes
{
	/// use template specialization to implement this function to your custom vertex type.
	/// To be interchangable with the other implementations,
	/// you should prefix the path with MRG::Folders::Rendering::meshesFolder
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> loadMeshFromFile(const char* filePath);

	template<Vertex VertexType>
	Ref<Mesh<VertexType>> quad();
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> circle()
	{
		return loadMeshFromFile<VertexType>("default/circle.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> cube()
	{
		return loadMeshFromFile<VertexType>("default/cube.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> sphere()
	{
		return loadMeshFromFile<VertexType>("default/sphere.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> cylinder()
	{
		return loadMeshFromFile<VertexType>("default/cylinder.obj");
	}
	template<Vertex VertexType>
	Ref<Mesh<VertexType>> torus()
	{
		return loadMeshFromFile<VertexType>("default/torus.obj");
	}
}  // namespace MRG::Utils::Meshes

#endif  // MORRIGU_DEFAULTMESHES_H
