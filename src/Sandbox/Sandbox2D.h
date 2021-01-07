#include <Morrigu.h>

class SandboxLayer : public MRG::Layer
{
public:
	SandboxLayer();
	~SandboxLayer() override = default;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(MRG::Timestep ts) override;
	void onImGuiRender() override;
	void onEvent(MRG::Event& event) override;

private:
	MRG::Ref<MRG::Texture2D> m_checkerboard, m_character;

	glm::vec4 m_color = {0.1f, 0.1f, 0.1f, 1.0f};
	MRG::Timestep m_frameTime;

	// temp
	MRG::Ref<MRG::Framebuffer> m_renderTarget;
};
