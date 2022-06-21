/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace diamond_square_terrain
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		castor3d::Parameters parameters;
	};

	enum class DiamondSquareSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'D', 'M', 'S', 'Q' ),
	};

	CU_DeclareAttributeParser( parserDiamondSquareTerrain )
	CU_DeclareAttributeParser( parserDiamondSquareTerrainEnd )
	CU_DeclareAttributeParser( parserRandomSeed )
	CU_DeclareAttributeParser( parserXzScale )
	CU_DeclareAttributeParser( parserUvScale )
	CU_DeclareAttributeParser( parserHeightRange )
	CU_DeclareAttributeParser( parserDetail )
}

#endif
