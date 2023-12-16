/*
See LICENSE file in root folder
*/
#ifndef ___Smaa_Parsers_H___
#define ___Smaa_Parsers_H___

#include "SmaaConfig.hpp"

#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace smaa
{
	struct SmaaContext
	{
		castor3d::RenderTargetRPtr renderTarget{};
		Preset preset{};
		SmaaConfig::Data data{};
	};

	enum class SmaaSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'S', 'M', 'A', 'A' ),
	};

	CU_DeclareAttributeParserBlock( parserSmaa, castor3d::TargetContext )
	CU_DeclareAttributeParserBlock( parserMode, SmaaContext )
	CU_DeclareAttributeParserBlock( parserPreset, SmaaContext )
	CU_DeclareAttributeParserBlock( parserEdgeDetection, SmaaContext )
	CU_DeclareAttributeParserBlock( parserDisableDiagonalDetection, SmaaContext )
	CU_DeclareAttributeParserBlock( parserDisableCornerDetection, SmaaContext )
	CU_DeclareAttributeParserBlock( parserThreshold, SmaaContext )
	CU_DeclareAttributeParserBlock( parserMaxSearchSteps, SmaaContext )
	CU_DeclareAttributeParserBlock( parserMaxSearchStepsDiag, SmaaContext )
	CU_DeclareAttributeParserBlock( parserCornerRounding, SmaaContext )
	CU_DeclareAttributeParserBlock( parserPredication, SmaaContext )
	CU_DeclareAttributeParserBlock( parserReprojection, SmaaContext )
	CU_DeclareAttributeParserBlock( parserReprojectionWeightScale, SmaaContext )
	CU_DeclareAttributeParserBlock( parserLocalContrastAdaptationFactor, SmaaContext )
	CU_DeclareAttributeParserBlock( parserPredicationScale, SmaaContext )
	CU_DeclareAttributeParserBlock( parserPredicationStrength, SmaaContext )
	CU_DeclareAttributeParserBlock( parserPredicationThreshold, SmaaContext )
	CU_DeclareAttributeParserBlock( parserSmaaEnd, SmaaContext )
}

#endif
