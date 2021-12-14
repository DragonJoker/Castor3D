/*
See LICENSE file in root folder
*/
#ifndef ___Water_Parsers_H___
#define ___Water_Parsers_H___

#include "WaterUbo.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace water
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		WaterUboConfiguration config;
		castor3d::Parameters parameters;
		std::vector< castor3d::MaterialRPtr > materials;
		uint32_t wave{};
	};

	enum class WaterSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'W', 'A', 'T', 'R' ),
	};

	CU_DeclareAttributeParser( parserWaterRendering )
	CU_DeclareAttributeParser( parserDampeningFactor )
	CU_DeclareAttributeParser( parserDepthSofteningDistance )
	CU_DeclareAttributeParser( parserRefrRatio )
	CU_DeclareAttributeParser( parserRefrDistortionFactor )
	CU_DeclareAttributeParser( parserRefrHeightFactor )
	CU_DeclareAttributeParser( parserRefrDistanceFactor )
	CU_DeclareAttributeParser( parserNormalMapScrollSpeed )
	CU_DeclareAttributeParser( parserNormalMapScroll )
	CU_DeclareAttributeParser( parserSsrSettings )
	CU_DeclareAttributeParser( parserNormals1 )
	CU_DeclareAttributeParser( parserNormals2 )
	CU_DeclareAttributeParser( parserNoise )
	CU_DeclareAttributeParser( parserWaterRenderingEnd )
}

#endif
