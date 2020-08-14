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

	// temp, should not be here
	MRG::Ref<MRG::VertexArray> m_vertexArray;
	MRG::Ref<MRG::Shader> m_shader;

	glm::vec4 m_color = {0.2f, 0.3f, 0.8f, 1.f};
};