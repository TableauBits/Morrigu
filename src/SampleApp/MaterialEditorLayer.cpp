//
// Created by Mathis Lamidey on 2021-06-26.
//

#include "MaterialEditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <utility>

void MaterialEditorLayer::setMaterial(MRG::Ref<MRG::Material<MRG::TexturedVertex>> newMaterial)
{
	m_editedMaterial = std::move(newMaterial);
	m_materialFloats.clear();
	m_materialVec2s.clear();
	m_materialVec3s.clear();
	m_materialVec4s.clear();

	const auto uniformData = m_editedMaterial->getUniformInfo(0);
	for (const auto& member : uniformData.members) { populateData(member); }
}

void MaterialEditorLayer::onImGuiUpdate(MRG::Timestep)
{
	m_floatIndex = 0;
	m_vec2Index  = 0;
	m_vec3Index  = 0;
	m_vec4Index  = 0;

	ImGui::Begin("Material editor");

	if (m_editedMaterial) {
		const auto uniformData = m_editedMaterial->getUniformInfo(0);
		if (ImGui::TreeNode(uniformData.name.c_str())) {
			for (const auto& member : uniformData.members) { renderData(member); }
			ImGui::TreePop();
		}
	} else {
		ImGui::Text("No material selected! Have you called \"MaterialEditorLayer::setMaterial\"?");
	}

	ImGui::End();
}

void MaterialEditorLayer::populateData(const MRG::Shader::Node& node)
{
	if (node.members.empty()) {
		switch (node.type.basetype) {
		case spirv_cross::SPIRType::BaseType::Float: {
			switch (node.type.vecsize) {
			case 1: {
				// Single float
				m_materialFloats.emplace_back();
			} break;
			case 2: {
				// vec2
				m_materialVec2s.emplace_back();
			} break;
			case 3: {
				// vec3
				m_materialVec3s.emplace_back();
			} break;
			case 4: {
				// vec4
				m_materialVec4s.emplace_back();
			} break;
			}
		} break;

		default: {
			MRG_WARN("Shader data type not supported yet!")
		} break;
		}
	} else {
		for (const auto& member : node.members) { populateData(member); }
	}
}

void MaterialEditorLayer::renderData(const MRG::Shader::Node& node)
{
	if (node.members.empty()) {
		switch (node.type.basetype) {
		case spirv_cross::SPIRType::BaseType::Float: {
			switch (node.type.vecsize) {
			case 1: {
				// Single float
				ImGui::DragFloat(node.name.c_str(), &m_materialFloats.at(m_floatIndex++));
			} break;
			case 2: {
				// vec2
				ImGui::DragFloat2(node.name.c_str(), glm::value_ptr(m_materialVec2s.at(m_vec2Index++)));
			} break;
			case 3: {
				// vec3
				ImGui::DragFloat3(node.name.c_str(), glm::value_ptr(m_materialVec3s.at(m_vec3Index++)));
			} break;
			case 4: {
				// vec4
				ImGui::DragFloat4(node.name.c_str(), glm::value_ptr(m_materialVec4s.at(m_vec4Index++)));
			} break;
			}
		} break;

		default: {
			MRG_WARN("Shader data type not supported yet!")
		} break;
		}
	} else {
		for (const auto& member : node.members) { renderData(member); }
	}
}
