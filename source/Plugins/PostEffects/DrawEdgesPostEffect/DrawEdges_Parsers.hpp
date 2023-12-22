/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_Parsers_H___
#define ___C3DDE_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace draw_edges
{
	castor::AttributeParsers createParsers();
	castor::StrUInt32Map createSections();
}

#endif
