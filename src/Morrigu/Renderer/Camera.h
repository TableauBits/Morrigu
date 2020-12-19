#ifndef MRG_CLASS_CAMERA
#define MRG_CLASS_CAMERA

#include "Core/GLMIncludeHelper.h"

namespace MRG
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection) : m_projection(projection) {}

		virtual ~Camera() = default;

		[[nodiscard]] const glm::mat4& getProjection() const { return m_projection; }

	protected:
		glm::mat4 m_projection = glm::mat4{1.f};
	};
}  // namespace MRG

#endif