#ifndef MRG_CLASS_ORTHOCAMERA
#define MRG_CLASS_ORTHOCAMERA

#include "Core/GLMIncludeHelper.h"

namespace MRG
{
	class OrthoCamera
	{
	public:
		OrthoCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		[[nodiscard]] const auto& getPosition() const { return m_position; }
		[[nodiscard]] const auto& getRotation() const { return m_rotation; }

		void setPosition(const glm::vec3& position);
		void setRotation(float position);

		[[nodiscard]] const auto& getProjectionMatrix() const { return m_projectionMatrix; }
		[[nodiscard]] const auto& getViewMatrix() const { return m_viewMatrix; }
		[[nodiscard]] const auto& getProjectionViewMatrix() const { return m_projectionViewMatrix; }

	private:
		void recalculateViewMatrix();

		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionViewMatrix;

		glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
		float m_rotation = 0.0f;
	};
}  // namespace MRG

#endif
