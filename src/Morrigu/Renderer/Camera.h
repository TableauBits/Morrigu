#ifndef MRG_CLASS_CAMERA
#define MRG_CLASS_CAMERA

#include "Core/GLMIncludeHelper.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const Camera&) = default;
		Camera(Camera&&) = default;
		explicit Camera(const glm::mat4& projection) : m_projection(projection) {}

		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) = default;

		virtual ~Camera() = default;

		[[nodiscard]] const glm::mat4& getProjection() const { return m_projection; }

		void setProjection(const glm::mat4& projection)
		{
			m_projection = projection;
			if (RenderingAPI::getAPI() == RenderingAPI::API::Vulkan) {
				m_projection[1][1] *= -1;
			}
		}

	private:
		glm::mat4 m_projection = glm::mat4{1.f};
	};
}  // namespace MRG

#endif
