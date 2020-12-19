#ifndef MRG_CLASS_CAMERA
#define MRG_CLASS_CAMERA

#include "Core/GLMIncludeHelper.h"

namespace MRG
{
	class Camera
	{
	public:
		Camera(const glm::mat4& projection) : m_projection(projection) {}

		[[nodiscard]] const glm::mat4& getProjection() const { return m_projection; }

	private:
		glm::mat4 m_projection;
	};
}  // namespace MRG

#endif