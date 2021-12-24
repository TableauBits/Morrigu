#include "AssetRegistry.h"

void AssetRegistry::addTexture(MRG::Ref<MRG::Texture> newTexture) { addTexture(newTexture->path, newTexture); }
void AssetRegistry::addTexture(const std::string& name, MRG::Ref<MRG::Texture> texture) { m_textures.emplace(name, texture); }
void AssetRegistry::addMesh(const std::string& name, MRG::Ref<MRG::Mesh<MRG::TexturedVertex>> mesh) { m_meshes.emplace(name, mesh); }

void AssetRegistry::removeTexture(const std::string& name) { m_textures.erase(name); }
void AssetRegistry::removeMesh(const std::string& name) { m_meshes.erase(name); }

std::optional<MRG::Ref<MRG::Texture>> AssetRegistry::getTexture(const std::string& name)
{
	std::optional<MRG::Ref<MRG::Texture>> texture{};

	if (m_textures.contains(name)) { texture = m_textures.at(name); }
	return texture;
}
std::optional<MRG::Ref<MRG::Mesh<MRG::TexturedVertex>>> AssetRegistry::getMesh(const std::string& name)
{
	std::optional<MRG::Ref<MRG::Mesh<MRG::TexturedVertex>>> mesh{};

	if (m_meshes.contains(name)) { mesh = m_meshes.at(name); }
	return mesh;
}
