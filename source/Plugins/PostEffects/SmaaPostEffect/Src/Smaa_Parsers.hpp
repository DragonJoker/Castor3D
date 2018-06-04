/*
See LICENSE file in root folder
*/
#ifndef ___Smaa_Parsers_H___
#define ___Smaa_Parsers_H___

#include <Castor3DPrerequisites.hpp>

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

namespace smaa
{
	enum class SmaaSection
		: uint32_t
	{
		eRoot = MAKE_SECTION_NAME( 'S', 'M', 'A', 'A' ),
	};

	DECLARE_ATTRIBUTE_PARSER( parserSmaa )
	DECLARE_ATTRIBUTE_PARSER( parserMode )
	DECLARE_ATTRIBUTE_PARSER( parserPreset )
	DECLARE_ATTRIBUTE_PARSER( parserEdgeDetection )
	DECLARE_ATTRIBUTE_PARSER( parserDisableDiagonalDetection )
	DECLARE_ATTRIBUTE_PARSER( parserDisableCornerDetection )
	DECLARE_ATTRIBUTE_PARSER( parserThreshold )
	DECLARE_ATTRIBUTE_PARSER( parserMaxSearchSteps )
	DECLARE_ATTRIBUTE_PARSER( parserMaxSearchStepsDiag )
	DECLARE_ATTRIBUTE_PARSER( parserCornerRounding )
	DECLARE_ATTRIBUTE_PARSER( parserReprojection )
	DECLARE_ATTRIBUTE_PARSER( parserReprojectionWeightScale )
	DECLARE_ATTRIBUTE_PARSER( parserLocalContrastAdaptationFactor )
	DECLARE_ATTRIBUTE_PARSER( parserPredicationScale )
	DECLARE_ATTRIBUTE_PARSER( parserPredicationStrength )
	DECLARE_ATTRIBUTE_PARSER( parserSmaaEnd )
}

#endif
