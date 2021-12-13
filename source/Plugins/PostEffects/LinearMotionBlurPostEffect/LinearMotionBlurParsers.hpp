/*
See LICENSE file in root folder
*/
#ifndef ___MotionBlur_Parsers_H___
#define ___MotionBlur_Parsers_H___

#include <Castor3D/Castor3DModule.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace motion_blur
{
	struct Configuration
	{
		uint32_t samplesCount{ 4u };
		float vectorDivider = 1.0f;
		float blurScale = 1.0f;
	};

	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Configuration data;
		bool fpsScale;
	};

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
