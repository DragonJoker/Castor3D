/*
See LICENSE file in root folder
*/
#ifndef ___MotionBlur_Parsers_H___
#define ___MotionBlur_Parsers_H___

#include <Castor3D/Castor3DPrerequisites.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace motion_blur
{
	enum class MotionBlurSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'M', 'T', 'B', 'R' ),
	};

	CU_DeclareAttributeParser( parserMotionBlur )
	CU_DeclareAttributeParser( parserDivider )
	CU_DeclareAttributeParser( parserSamples )
	CU_DeclareAttributeParser( parserFpsScale )
	CU_DeclareAttributeParser( parserMotionBlurEnd )
}

#endif
