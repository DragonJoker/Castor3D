/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace diamond_square_terrain
{
	c3d::AttributeParsers createParsers();
	c3d::StrUInt32Map createSections();
}

#endif
