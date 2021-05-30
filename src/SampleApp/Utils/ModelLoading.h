//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_MODELLOADING_H
#define MORRIGU_MODELLOADING_H

#include <Morrigu.h>

namespace Utils
{
	MRG::Ref<MRG::Mesh<MRG::ColoredVertex>> loadMeshFromFileVertexColored(const char* filePath);
	MRG::Ref<MRG::Mesh<MRG::BasicVertex>> loadMeshFromFileBasicVertex(const char* filePath);
};  // namespace Utils

#endif  // MORRIGU_MODELLOADING_H
