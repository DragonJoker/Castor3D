/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_HairRendering_Parsers_H___
#define __C3DAS_HairRendering_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace hair
{
	c3d::AttributeParsers createParsers();
	c3d::StrSectionIdMap createSections();
}

#endif
