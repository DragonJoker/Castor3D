#include "SmaaPostEffect/Smaa_Parsers.hpp"

#include "SmaaPostEffect/SmaaPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace smaa
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Preset preset{};
		SmaaConfig::Data data{};
	};

	ParserContext & getParserContext( castor::FileParserContextSPtr context )
	{
		return *static_cast< ParserContext * >( context->getUserContext( PostEffect::Type ) );
	}

	CU_ImplementAttributeParser( parserSmaa )
	{
		ParserContext * smaaContext = new ParserContext;
		smaaContext->engine = std::static_pointer_cast< castor3d::SceneFileContext >( context )->m_pParser->getEngine();
		context->registerUserContext( PostEffect::Type, smaaContext );
	}
	CU_EndAttributePush( SmaaSection::eRoot )

	CU_ImplementAttributeParser( parserMode )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			smaaContext.data.mode = Mode( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPreset )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			smaaContext.preset = Preset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEdgeDetection )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			smaaContext.data.edgeDetection = EdgeDetectionType( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDisableDiagonalDetection )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.disableDiagonalDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDisableCornerDetection )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.disableCornerDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserThreshold )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.threshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaxSearchSteps )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.maxSearchSteps );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaxSearchStepsDiag )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.maxSearchStepsDiag );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCornerRounding )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.cornerRounding );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserPredication )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.enablePredication );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserReprojection )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.enableReprojection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserReprojectionWeightScale )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.reprojectionWeightScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLocalContrastAdaptationFactor )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.localContrastAdaptationFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationScale )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.predicationScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationStrength )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.predicationStrength );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPredicationThreshold )
	{
		auto & smaaContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( smaaContext.data.predicationThreshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSmaaEnd )
	{
		auto & smaaContext = getParserContext( context );
		auto engine = smaaContext.engine;
		auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "mode" ), getName( smaaContext.data.mode ) );
		parameters.add( cuT( "preset" ), getName( smaaContext.preset ) );
		parameters.add( cuT( "edgeDetection" ), getName( smaaContext.data.edgeDetection ) );
		parameters.add( cuT( "disableDiagonalDetection" ), smaaContext.data.disableDiagonalDetection );
		parameters.add( cuT( "disableCornerDetection" ), smaaContext.data.disableCornerDetection );
		parameters.add( cuT( "localContrastAdaptationFactor" ), smaaContext.data.localContrastAdaptationFactor );
		parameters.add( cuT( "enablePredication" ), smaaContext.data.enablePredication );
		parameters.add( cuT( "predicationScale" ), smaaContext.data.predicationScale );
		parameters.add( cuT( "predicationStrength" ), smaaContext.data.predicationStrength );
		parameters.add( cuT( "predicationThreshold" ), smaaContext.data.predicationThreshold );

		if ( smaaContext.preset == Preset::eCustom )
		{
			parameters.add( cuT( "threshold" ), smaaContext.data.threshold );
			parameters.add( cuT( "maxSearchSteps" ), smaaContext.data.maxSearchSteps );
			parameters.add( cuT( "maxSearchStepsDiag" ), smaaContext.data.maxSearchStepsDiag );
			parameters.add( cuT( "cornerRounding" ), smaaContext.data.cornerRounding );
		}

		if ( smaaContext.data.mode == Mode::eT2X )
		{
			parameters.add( cuT( "enableReprojection" ), smaaContext.data.enableReprojection );
			parameters.add( cuT( "reprojectionWeightScale" ), smaaContext.data.reprojectionWeightScale );
		}

		auto effect = engine->getRenderTargetCache().getPostEffectFactory().create( PostEffect::Type
			, *parsingContext->renderTarget
			, *engine->getRenderSystem()
			, parameters );
		parsingContext->renderTarget->addPostEffect( effect );

		delete reinterpret_cast< ParserContext * >( context->unregisterUserContext( PostEffect::Type ) );
	}
	CU_EndAttributePop()
}
