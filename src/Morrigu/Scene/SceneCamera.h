#ifndef MRG_CLASS_SCENECAMERA
#define MRG_CLASS_SCENECAMERA

#include "Renderer/Camera.h"

namespace MRG
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Orthographic = 0,
			Perspective = 1
		};

		SceneCamera();
		SceneCamera(const SceneCamera&) = default;
		SceneCamera(SceneCamera&&) = default;
		~SceneCamera() override = default;

		SceneCamera& operator=(const SceneCamera&) = default;
		SceneCamera& operator=(SceneCamera&&) = default;

		void setOrthographic(float size, float nearClip, float farClip);
		void setPerspective(float verticalFOV, float nearClip, float farClip);

		[[nodiscard]] ProjectionType getProjectionType() const { return m_projectionType; }
		void setProjectionType(ProjectionType type)
		{
			m_projectionType = type;
			recalculateProjection();
		}

		void setViewportSize(uint32_t width, uint32_t height);

		[[nodiscard]] float getOrthographicSize() const { return m_orthographicSize; }
		void setOrthographicSize(float orthographicSize)
		{
			m_orthographicSize = orthographicSize;
			recalculateProjection();
		}
		[[nodiscard]] float getOrthographicNear() const { return m_orthographicNear; }
		void setOrthographicNear(float orthographicNear)
		{
			m_orthographicNear = orthographicNear;
			recalculateProjection();
		}
		[[nodiscard]] float getOrthographicFar() const { return m_orthographicFar; }
		void setOrthographicFar(float orthographicFar)
		{
			m_orthographicFar = orthographicFar;
			recalculateProjection();
		}

		[[nodiscard]] float getPerspectiveFOV() const { return m_perspectiveFOV; }
		void setPerspectiveFOV(float perspectiveFOV)
		{
			m_perspectiveFOV = perspectiveFOV;
			recalculateProjection();
		}
		[[nodiscard]] float getPerspectiveNear() const { return m_perspectiveNear; }
		void setPerspectiveNear(float perspectiveNear)
		{
			m_perspectiveNear = perspectiveNear;
			recalculateProjection();
		}
		[[nodiscard]] float getPerspectiveFar() const { return m_perspectiveFar; }
		void setPerspectiveFar(float perspectiveFar)
		{
			m_perspectiveFar = perspectiveFar;
			recalculateProjection();
		}

	private:
		void recalculateProjection();

		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_orthographicSize = 10.f;
		float m_orthographicNear = -1.f, m_orthographicFar = 1.f;

		float m_perspectiveFOV = glm::radians(45.f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.f;

		float m_aspectRatio = 0.f;
	};
}  // namespace MRG

#endif
