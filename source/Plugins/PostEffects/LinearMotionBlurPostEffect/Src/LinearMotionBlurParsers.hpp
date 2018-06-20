/*
See LICENSE file in root folder
*/
#ifndef ___MotionBlur_Parsers_H___
#define ___MotionBlur_Parsers_H___

#include <Castor3DPrerequisites.hpp>

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

namespace motion_blur
{
	enum class MotionBlurSection
		: uint32_t
	{
		eRoot = MAKE_SECTION_NAME( 'M', 'T', 'B', 'R' ),
	};

	DECLARE_ATTRIBUTE_PARSER( parserMotionBlur )
	DECLARE_ATTRIBUTE_PARSER( parserDivider )
	DECLARE_ATTRIBUTE_PARSER( parserSamples )
	DECLARE_ATTRIBUTE_PARSER( parserFpsScale )
	DECLARE_ATTRIBUTE_PARSER( parserMotionBlurEnd )
}

#endif
