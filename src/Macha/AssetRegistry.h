#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include <Morrigu.h>

#include <optional>
#include <unordered_map>

class AssetRegistry
{
public:
	// When using the add functions, if a value with the same name is already present, it will be overwritten
	void addTexture(MRG::Ref<MRG::Texture> newTexture);
	void addTexture(const std::string& name, MRG::Ref<MRG::Texture> texture);
	void addMesh(const std::string& name, MRG::Ref<MRG::Mesh<MRG::TexturedVertex>> mesh);

	void removeTexture(const std::string& name);
	void removeMesh(const std::string& name);

	[[nodiscard]] std::optional<MRG::Ref<MRG::Texture>> getTexture(const std::string& name);
	[[nodiscard]] std::optional<MRG::Ref<MRG::Mesh<MRG::TexturedVertex>>> getMesh(const std::string& name);

private:
	std::unordered_map<std::string, MRG::Ref<MRG::Texture>> m_textures{};
	std::unordered_map<std::string, MRG::Ref<MRG::Mesh<MRG::TexturedVertex>>> m_meshes{};
};

#endif
