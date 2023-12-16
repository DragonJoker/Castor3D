/*
See LICENSE file in root folder
*/
#ifndef ___MotionBlur_Parsers_H___
#define ___MotionBlur_Parsers_H___

#include <Castor3D/Scene/SceneFileParser.hpp>

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

	struct BlurContext
	{
		castor3d::RenderTargetRPtr renderTarget{};
		Configuration data{};
		bool fpsScale{};
	};

	enum class MotionBlurSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'M', 'T', 'B', 'R' ),
	};

	CU_DeclareAttributeParserBlock( parserMotionBlur, castor3d::TargetContext )
	CU_DeclareAttributeParserBlock( parserDivider, BlurContext )
	CU_DeclareAttributeParserBlock( parserSamples, BlurContext )
	CU_DeclareAttributeParserBlock( parserFpsScale, BlurContext )
	CU_DeclareAttributeParserBlock( parserMotionBlurEnd, BlurContext )
}

#endif
