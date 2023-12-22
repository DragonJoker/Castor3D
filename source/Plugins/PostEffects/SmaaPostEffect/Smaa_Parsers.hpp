/*
See LICENSE file in root folder
*/
#ifndef ___Smaa_Parsers_H___
#define ___Smaa_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace smaa
{
	castor::AttributeParsers createParsers();
	castor::StrUInt32Map createSections();
}

#endif
