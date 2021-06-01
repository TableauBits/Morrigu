//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_MODELLOADING_H
#define MORRIGU_MODELLOADING_H

#include "Rendering/Mesh.h"

namespace MRG::Utils
{
	/// These functions are samples. They already point to the shader folder, so no path should be necessary.
	Ref<Mesh<BasicVertex>> loadMeshFromFileBasicVertex(const char* filePath);
	Ref<Mesh<ColoredVertex>> loadMeshFromFileColoredVertex(const char* filePath);
	Ref<Mesh<TexturedVertex>> loadMeshFromFileTexturedVertex(const char* filePath);
};  // namespace MRG::Utils

#endif  // MORRIGU_MODELLOADING_H
