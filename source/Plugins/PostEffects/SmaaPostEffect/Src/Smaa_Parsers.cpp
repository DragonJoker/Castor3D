#include "Smaa_Parsers.hpp"

#include "SmaaPostEffect.hpp"

#include <Engine.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Material/Material.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/SceneFileParser.hpp>

#include <stack>

namespace smaa
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Preset preset;
		SmaaConfig::Data data;
	};

	ParserContext & getParserContext( castor::FileParserContextSPtr context )
	{
		return *static_cast< ParserContext * >( context->getUserContext( PostEffect::Type ) );
	}

	CU_ImplementAttributeParser( parserSmaa )
	{
		ParserContext * context = new ParserContext;
		context->engine = std::static_pointer_cast< castor3d::SceneFileContext >( p_context )->m_pParser->getEngine();
		p_context->registerUserContext( PostEffect::Type, context );
	}
	CU_EndAttributePush( SmaaSection::eRoot )

	CU_ImplementAttributeParser( parserMode )
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
			context.data.mode = Mode( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPreset )
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
			context.preset = Preset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEdgeDetection )
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
			context.data.edgeDetection = EdgeDetectionType( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDisableDiagonalDetection )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.disableDiagonalDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDisableCornerDetection )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.disableCornerDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserThreshold )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.threshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaxSearchSteps )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.maxSearchSteps );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaxSearchStepsDiag )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.maxSearchStepsDiag );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCornerRounding )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.cornerRounding );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserPredication )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.enablePredication );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserReprojection )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.enableReprojection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserReprojectionWeightScale )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.reprojectionWeightScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLocalContrastAdaptationFactor )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.localContrastAdaptationFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationScale )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.predicationScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationStrength )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.predicationStrength );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationThreshold )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.data.predicationThreshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSmaaEnd )
	{
		auto & context = getParserContext( p_context );
		auto engine = context.engine;
		auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( p_context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "mode" ), getName( context.data.mode ) );
		parameters.add( cuT( "preset" ), getName( context.preset ) );
		parameters.add( cuT( "edgeDetection" ), getName( context.data.edgeDetection ) );
		parameters.add( cuT( "disableDiagonalDetection" ), context.data.disableDiagonalDetection );
		parameters.add( cuT( "disableCornerDetection" ), context.data.disableCornerDetection );
		parameters.add( cuT( "localContrastAdaptationFactor" ), context.data.localContrastAdaptationFactor );
		parameters.add( cuT( "enablePredication" ), context.data.enablePredication );
		parameters.add( cuT( "predicationScale" ), context.data.predicationScale );
		parameters.add( cuT( "predicationStrength" ), context.data.predicationStrength );
		parameters.add( cuT( "predicationThreshold" ), context.data.predicationThreshold );

		if ( context.preset == Preset::eCustom )
		{
			parameters.add( cuT( "threshold" ), context.data.threshold );
			parameters.add( cuT( "maxSearchSteps" ), context.data.maxSearchSteps );
			parameters.add( cuT( "maxSearchStepsDiag" ), context.data.maxSearchStepsDiag );
			parameters.add( cuT( "cornerRounding" ), context.data.cornerRounding );
		}

		if ( context.data.mode == Mode::eT2X )
		{
			parameters.add( cuT( "enableReprojection" ), context.data.enableReprojection );
			parameters.add( cuT( "reprojectionWeightScale" ), context.data.reprojectionWeightScale );
		}

		auto effect = engine->getRenderTargetCache().getPostEffectFactory().create( PostEffect::Type
			, *parsingContext->renderTarget
			, *engine->getRenderSystem()
			, parameters );
		parsingContext->renderTarget->addPostEffect( effect );

		delete reinterpret_cast< ParserContext * >( p_context->unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()
}
