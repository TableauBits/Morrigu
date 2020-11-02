#include <Morrigu.h>

class Sandbox2D : public MRG::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(MRG::Timestep ts) override;
	void onImGuiRender() override;
	void onEvent(MRG::Event& event) override;

private:
	MRG::OrthoCameraController m_camera;

	MRG::Ref<MRG::Texture2D> m_checkerboard, m_character;

	glm::vec4 m_color = {0.1f, 0.1f, 0.1f, 1.0f};
	float m_fps = 0;
};