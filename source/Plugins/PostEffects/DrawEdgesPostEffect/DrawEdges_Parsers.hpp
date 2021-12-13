/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_Parsers_H___
#define ___C3DDE_Parsers_H___

#include "DrawEdgesUbo.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace draw_edges
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		DrawEdgesUboConfiguration data{};
	};

	enum class Section
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'D', 'R', 'E', 'D' ),
	};

	CU_DeclareAttributeParser( parserDrawEdges )
	CU_DeclareAttributeParser( parserNormalDepthWidth )
	CU_DeclareAttributeParser( parserObjectWidth )
	CU_DeclareAttributeParser( parserDrawEdgesEnd )
}

#endif
