#include "LinearMotionBlurPostEffect/LinearMotionBlurParsers.hpp"

#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffectFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace motion_blur
{
	namespace parse
	{
		struct BlurContext
		{
			c3d::RenderTargetRPtr renderTarget{};
			Configuration data{};
			bool fpsScale{};
		};

		enum class MotionBlurSection
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'M', 'T', 'B', 'R' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserMotionBlur, c3d::TargetContext, BlurContext )
		{
			newBlockContext->renderTarget = blockContext->renderTarget;
		}
		CU_EndAttributePushNewBlock( MotionBlurSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserDivider, BlurContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.vectorDivider );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamples, BlurContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.samplesCount );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFpsScale, BlurContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->fpsScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMotionBlurEnd, BlurContext )
		{
			c3d::Parameters parameters;
			parameters.add( cuT( "vectorDivider" ), blockContext->data.vectorDivider );
			parameters.add( cuT( "samplesCount" ), blockContext->data.samplesCount );
			parameters.add( cuT( "fpsScale" ), blockContext->fpsScale );

			auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
			effect->enable( true );
			effect->setParameters( parameters );
		}
		CU_EndAttributePop()
	}

	c3d::AttributeParsers createParsers()
	{
		c3d::AttributeParsers result;

		addParserT( result
			, c3d::CSCNSection::eRenderTarget
			, parse::MotionBlurSection::eRoot
			, cuT( "linear_motion_blur" )
			, &parse::parserMotionBlur );

		addParserT( result
			, parse::MotionBlurSection::eRoot
			, cuT( "vectorDivider" )
			, &parse::parserDivider, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::MotionBlurSection::eRoot
			, cuT( "samples" )
			, &parse::parserSamples, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::MotionBlurSection::eRoot
			, cuT( "fpsScale" )
			, &parse::parserFpsScale, { c3d::makeParameter< c3d::ParameterType::eBool >() } );
		addParserT( result
			, parse::MotionBlurSection::eRoot
			, c3d::CSCNSection::eRenderTarget
			, cuT( "}" )
			, &parse::parserMotionBlurEnd );

		return result;
	}

	c3d::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( parse::MotionBlurSection::eRoot ), cuT( "motion_blur" ) },
		};
	}
}
