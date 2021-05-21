//
// Created by Mathis Lamidey on 2021-05-18.
//

#include "RenderObject.h"

#include <utility>

namespace MRG
{
	Ref<RenderObject> RenderObject::create(const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		return createRef<RenderObject>(RenderObject{mesh, material});
	}

	void RenderObject::rotate(const glm::vec3& axis, float radRotation) { modelMatrix = glm::rotate(modelMatrix, radRotation, axis); }
	void RenderObject::scale(const glm::vec3& scaling) { modelMatrix = glm::scale(modelMatrix, scaling); }
	void RenderObject::translate(const glm::vec3& translation) { modelMatrix = glm::translate(modelMatrix, translation); }
	RenderObject::RenderObject(Ref<Mesh> mesh, Ref<Material> material) : mesh{std::move(mesh)}, material{std::move(material)} {}

}  // namespace MRG
