#include "LinearMotionBlurParsers.hpp"

#include "LinearMotionBlurPostEffect.hpp"

#include <Engine.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Material/Material.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/SceneFileParser.hpp>

#include <stack>

namespace motion_blur
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Configuration data;
		bool fpsScale;
	};

	ParserContext & getParserContext( castor::FileParserContextSPtr context )
	{
		return *static_cast< ParserContext * >( context->getUserContext( PostEffect::Type ) );
	}

	CU_ImplementAttributeParser( parserMotionBlur )
	{
		ParserContext * context = new ParserContext;
		context->engine = std::static_pointer_cast< castor3d::SceneFileContext >( p_context )->m_pParser->getEngine();
		p_context->registerUserContext( PostEffect::Type, context );
	}
	CU_EndAttributePush( MotionBlurSection::eRoot )

	CU_ImplementAttributeParser( parserDivider )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			float value{ 0.0f };
			p_params[0]->get( value );
			context.data.vectorDivider = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamples )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			p_params[0]->get( value );
			context.data.samplesCount = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFpsScale )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			bool value{ 0u };
			p_params[0]->get( value );
			context.fpsScale = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMotionBlurEnd )
	{
		auto & context = getParserContext( p_context );
		auto engine = context.engine;
		auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( p_context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "vectorDivider" ), context.data.vectorDivider );
		parameters.add( cuT( "samplesCount" ), context.data.samplesCount );
		parameters.add( cuT( "fpsScale" ), context.fpsScale );

		auto effect = engine->getRenderTargetCache().getPostEffectFactory().create( PostEffect::Type
			, *parsingContext->renderTarget
			, *engine->getRenderSystem()
			, parameters );
		parsingContext->renderTarget->addPostEffect( effect );

		delete reinterpret_cast< ParserContext * >( p_context->unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()
}
