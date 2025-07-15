/*
See LICENSE file in root folder
*/
#ifndef ___Smaa_Parsers_H___
#define ___Smaa_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace smaa
{
	c3d::AttributeParsers createParsers();
	c3d::StrUInt32Map createSections();
}

#endif
