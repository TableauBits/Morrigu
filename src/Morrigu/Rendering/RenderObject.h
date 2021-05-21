//
// Created by Mathis Lamidey on 2021-05-18.
//

#ifndef MORRIGU_RENDEROBJECT_H
#define MORRIGU_RENDEROBJECT_H

#include "Rendering/Material.h"
#include "Rendering/Mesh.h"

#include "Rendering/GLMIncludeHelper.h"

namespace MRG
{
	class RenderObject
	{
	public:
		static Ref<RenderObject> create(const Ref<Mesh>& mesh, const Ref<Material>& material);

		Ref<Mesh> mesh;
		Ref<Material> material;
		glm::mat4 modelMatrix{1.f};
		bool isVisible{true};

		void rotate(const glm::vec3& axis, float radRotation);
		void scale(const glm::vec3& scaling);
		void translate(const glm::vec3& translation);

	private:
		RenderObject(Ref<Mesh> mesh, Ref<Material> material);
	};
}  // namespace MRG

#endif  // MORRIGU_RENDEROBJECT_H
