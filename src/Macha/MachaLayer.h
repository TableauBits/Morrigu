#include <Morrigu.h>

namespace MRG
{
	class MachaLayer : public Layer
	{
	public:
		MachaLayer();
		virtual ~MachaLayer() = default;

		void onAttach() override;
		void onDetach() override;

		void onUpdate(Timestep ts) override;
		void onImGuiRender() override;
		void onEvent(Event& event) override;

	private:
		OrthoCameraController m_camera;

		Ref<Texture2D> m_checkerboard, m_character;

		glm::vec2 m_viewportSize = {0.f, 0.f};

		glm::vec4 m_color = {0.1f, 0.1f, 0.1f, 1.0f};
		Timestep m_frameTime;

		// temp
		Ref<Framebuffer> m_renderTarget;
	};
}  // namespace MRG
