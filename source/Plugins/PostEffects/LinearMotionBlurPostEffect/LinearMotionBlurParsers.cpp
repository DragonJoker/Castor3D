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
	namespace
	{
		ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( PostEffect::Type ) );
		}
	}

	CU_ImplementAttributeParser( parserMotionBlur )
	{
	}
	CU_EndAttributePush( MotionBlurSection::eRoot )

	CU_ImplementAttributeParser( parserDivider )
	{
		auto & blurContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			float value{ 0.0f };
			params[0]->get( value );
			blurContext.data.vectorDivider = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamples )
	{
		auto & blurContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			blurContext.data.samplesCount = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFpsScale )
	{
		auto & blurContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value{ 0u };
			params[0]->get( value );
			blurContext.fpsScale = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMotionBlurEnd )
	{
		auto & blurContext = getParserContext( context );
		auto & parsingContext = castor3d::getParserContext( context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "vectorDivider" ), blurContext.data.vectorDivider );
		parameters.add( cuT( "samplesCount" ), blurContext.data.samplesCount );
		parameters.add( cuT( "fpsScale" ), blurContext.fpsScale );

		auto effect = parsingContext.renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );

		delete reinterpret_cast< ParserContext * >( context.unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()
}
