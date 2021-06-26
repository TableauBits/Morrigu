//
// Created by Mathis Lamidey on 2021-06-26.
//

#ifndef MATERIALEDITORLAYER_H
#define MATERIALEDITORLAYER_H

#include <glm/glm.hpp>
#include <Morrigu.h>

#include <vector>

class MaterialEditorLayer : public MRG::Layer
{
public:
	void setMaterial(MRG::Ref<MRG::Material<MRG::TexturedVertex>> newMaterial);

	void onImGuiUpdate(MRG::Timestep ts) override;

private:
	void populateData(const MRG::Shader::Node& node);
	void renderData(const MRG::Shader::Node& node);

	MRG::Ref<MRG::Material<MRG::TexturedVertex>> m_editedMaterial;
	std::vector<float> m_materialFloats;
	std::vector<glm::vec2> m_materialVec2s;
	std::vector<glm::vec3> m_materialVec3s;
	std::vector<glm::vec4> m_materialVec4s;

	std::size_t m_floatIndex;
	std::size_t m_vec2Index;
	std::size_t m_vec3Index;
	std::size_t m_vec4Index;
};

#endif  // MATERIALEDITORLAYER_H
