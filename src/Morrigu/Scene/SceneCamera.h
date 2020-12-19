#ifndef MRG_CLASS_SCENECAMERA
#define MRG_CLASS_SCENECAMERA

#include "Renderer/Camera.h"

namespace MRG
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setOrthographic(float size, float nearClip, float farClip);

		void setViewportSize(uint32_t width, uint32_t height);

		float getOrthographicSize() const { return m_orthographicSize; }
		void setOrthographicSize(float orthographicSize)
		{
			m_orthographicSize = orthographicSize;
			recalculateProjection();
		}

	private:
		void recalculateProjection();

		float m_orthographicSize = 10.f;
		float m_orthographicNear = -1.f, m_orthographicFar = 1.f;
		float m_aspectRatio = 0.f;
	};
}  // namespace MRG

#endif
