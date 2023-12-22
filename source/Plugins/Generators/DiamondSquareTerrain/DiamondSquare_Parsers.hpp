/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace diamond_square_terrain
{
	castor::AttributeParsers createParsers();
	castor::StrUInt32Map createSections();
}

#endif
