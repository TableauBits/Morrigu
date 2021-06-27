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
	void renderData(const MRG::Shader::Node& node);

	MRG::Ref<MRG::Material<MRG::TexturedVertex>> m_editedMaterial;

	std::vector<std::vector<std::byte>> m_data;
	std::byte* m_rwHead;
};

#endif  // MATERIALEDITORLAYER_H
