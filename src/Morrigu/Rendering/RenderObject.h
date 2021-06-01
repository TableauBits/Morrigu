//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_RENDEROBJECT_H
#define MORRIGU_RENDEROBJECT_H

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

#include "Utils/GLMIncludeHelper.h"

namespace MRG
{
	struct RenderData
	{
		// mesh data
		vk::Buffer vertexBuffer;
		std::size_t vertexNumber{};

		// material data
		vk::PipelineLayout materialPipelineLayout;
		vk::Pipeline materialPipeline;
		vk::DescriptorSet level2Descriptor;

		// RO data
		Ref<glm::mat4> modelMatrix{};
		Ref<bool> isVisible{};
	};

	template<Vertex VertexType>
	class RenderObject
	{
	public:
		static Ref<RenderObject> create(const Ref<Mesh<VertexType>>& mesh, const Ref<Material<VertexType>>& material)
		{
			return createRef<RenderObject>(RenderObject{mesh, material});
		}

		Ref<Mesh<VertexType>> mesh;
		Ref<Material<VertexType>> material;
		Ref<glm::mat4> modelMatrix{createRef<glm::mat4>(1.f)};
		Ref<bool> isVisible{createRef<bool>(true)};

		void rotate(const glm::vec3& axis, float radRotation) { *modelMatrix = glm::rotate(*modelMatrix, radRotation, axis); }
		void scale(const glm::vec3& scaling) { *modelMatrix = glm::scale(*modelMatrix, scaling); }
		void translate(const glm::vec3& translation) { *modelMatrix = glm::translate(*modelMatrix, translation); }
		void setVisible(bool visible) { *isVisible = visible; }

		RenderData getRenderData()
		{
			return RenderData{
			  .vertexBuffer           = mesh->vertexBuffer.buffer,
			  .vertexNumber           = mesh->vertices.size(),
			  .materialPipelineLayout = material->pipelineLayout,
			  .materialPipeline       = material->pipeline,
			  .level2Descriptor       = material->level2Descriptor,
			  .modelMatrix            = modelMatrix,
			  .isVisible              = isVisible,
			};
		}

	private:
		RenderObject(Ref<Mesh<VertexType>> mesh, Ref<Material<VertexType>> material) : mesh{std::move(mesh)}, material{std::move(material)}
		{}
	};
}  // namespace MRG

#endif  // MORRIGU_RENDEROBJECT_H
