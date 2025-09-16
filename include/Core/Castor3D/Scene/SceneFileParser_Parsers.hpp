/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_Parsers_H___
#define ___C3D_SceneFileParser_Parsers_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	C3D_API AdditionalParsers createSceneFileParsers( Engine const & engine );
	C3D_API StrSectionIdMap registerSceneFileSections();
	C3D_API uint32_t getSceneFileRootSection();
}

#endif
