//
// Created by Mathis Lamidey on 2021-06-26.
//

#include "MaterialEditorLayer.h"

#include <imgui.h>

#include <utility>

void MaterialEditorLayer::setMaterial(MRG::Ref<MRG::Material<MRG::TexturedVertex>> newMaterial)
{
	m_data.clear();
	m_editedMaterial = std::move(newMaterial);

	m_data.resize(m_editedMaterial->shader->l2UBOData.size());
	auto index = 0;
	for (const auto& ubo : m_editedMaterial->shader->l2UBOData) {
		m_data[index].resize(ubo.second.size);
		++index;
	}
}

void MaterialEditorLayer::onImGuiUpdate(MRG::Timestep)
{
	ImGui::Begin("Material editor");

	if (m_editedMaterial) {
		auto index = 0;
		for (const auto& ubo : m_editedMaterial->shader->l2UBOData) {
			m_rwHead = m_data[index].data();
			if (ImGui::TreeNode(ubo.second.name.c_str())) {
				for (const auto& member : ubo.second.members) { renderData(member); }
				ImGui::TreePop();
			}

			m_editedMaterial->uploadUniform(index, m_data[index].data(), m_data[index].size());
			++index;
		}
	} else {
		ImGui::Text("No material selected! Have you called \"MaterialEditorLayer::setMaterial\"?");
	}

	ImGui::End();
}

void MaterialEditorLayer::renderData(const MRG::Shader::Node& node)
{
	if (node.members.empty()) {
		switch (node.type.basetype) {
		case spirv_cross::SPIRType::BaseType::Float: {
			auto* pointerCast = reinterpret_cast<float*>(m_rwHead);
			switch (node.type.vecsize) {
			case 1: {
				// Single float
				ImGui::DragFloat(node.name.c_str(), pointerCast);
				m_rwHead += 1 * sizeof(float);
			} break;
			case 2: {
				// vec2
				ImGui::DragFloat2(node.name.c_str(), pointerCast);
				m_rwHead += 2 * sizeof(float);
			} break;
			case 4: {
				// vec4
				ImGui::ColorEdit4(node.name.c_str(), pointerCast);
				m_rwHead += 4 * sizeof(float);
			} break;
			default: {
				MRG_WARN("Only scalar float and vec2/4 are supported!")
			} break;
			}
		} break;

		default: {
			MRG_WARN("Shader data type not supported yet!")
		} break;
		}
	} else {
		if (ImGui::TreeNode(node.name.c_str())) {
			for (const auto& member : node.members) { renderData(member); }
			ImGui::TreePop();
		}
	}
}
