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
	namespace
	{
		castor::String doGetName( Mode mode )
		{
			castor::String result;

			switch ( mode )
			{
			case Mode::e1X:
				result = cuT( "1X" );
				break;

			case Mode::eT2X:
				result = cuT( "T2X" );
				break;

			case Mode::eS2X:
				result = cuT( "S2X" );
				break;

			case Mode::e4X:
				result = cuT( "4X" );
				break;
			}

			return result;
		}

		castor::String doGetName( Preset preset )
		{
			castor::String result;

			switch ( preset )
			{
			case Preset::eLow:
				result = cuT( "low" );
				break;

			case Preset::eMedium:
				result = cuT( "medium" );
				break;

			case Preset::eHigh:
				result = cuT( "high" );
				break;

			case Preset::eUltra:
				result = cuT( "ultra" );
				break;

			case Preset::eCustom:
				result = cuT( "custom" );
				break;
			}

			return result;
		}

		castor::String doGetName( EdgeDetection detection )
		{
			castor::String result;

			switch ( detection )
			{
			case EdgeDetection::eDepth:
				result = cuT( "depth" );
				break;

			case EdgeDetection::eColour:
				result = cuT( "colour" );
				break;

			case EdgeDetection::eLuma:
				result = cuT( "luma" );
				break;
			}

			return result;
		}
	}

	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		Mode mode{ Mode::e1X };
		Preset preset{ Preset::eCustom };
		EdgeDetection detection{ EdgeDetection::eLuma };
		float threshold{ 0.1f };
		int maxSearchSteps{ 16 };
		int maxSearchStepsDiag{ 8 };
		int cornerRounding{ 100 };
		bool reprojection{ false };
		float reprojectionWeightScale{ 30.0f };
	};

	ParserContext & getParserContext( castor::FileParserContextSPtr context )
	{
		return *static_cast< ParserContext * >( context->getUserContext( PostEffect::Type ) );
	}

	IMPLEMENT_ATTRIBUTE_PARSER( parserSmaa )
	{
		ParserContext * context = new ParserContext;
		context->engine = std::static_pointer_cast< castor3d::SceneFileContext >( p_context )->m_pParser->getEngine();
		p_context->registerUserContext( PostEffect::Type, context );
	}
	END_ATTRIBUTE_PUSH( SmaaSection::eRoot )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMode )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			p_params[0]->get( value );
			context.mode = Mode( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPreset )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			p_params[0]->get( value );
			context.preset = Preset( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserEdgeDetection )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			p_params[0]->get( value );
			context.detection = EdgeDetection( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserThreshold )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.threshold );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMaxSearchSteps )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.maxSearchSteps );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMaxSearchStepsDiag )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.maxSearchStepsDiag );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserCornerRounding )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.cornerRounding );
		}
	}
	END_ATTRIBUTE()
		
	IMPLEMENT_ATTRIBUTE_PARSER( parserReprojection )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.reprojection );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserReprojectionWeightScale )
	{
		auto & context = getParserContext( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( "Missing parameter" );
		}
		else
		{
			p_params[0]->get( context.reprojectionWeightScale );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSmaaEnd )
	{
		auto & context = getParserContext( p_context );
		auto engine = context.engine;
		auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( p_context );
		castor3d::Parameters parameters;
		parameters.add( cuT( "mode" ), doGetName( context.mode ) );
		parameters.add( cuT( "preset" ), doGetName( context.preset ) );
		parameters.add( cuT( "edgeDetection" ), doGetName( context.detection ) );

		if ( context.preset == Preset::eCustom )
		{
			parameters.add( cuT( "threshold" ), context.threshold );
			parameters.add( cuT( "maxSearchSteps" ), context.maxSearchSteps );
			parameters.add( cuT( "maxSearchStepsDiag" ), context.maxSearchStepsDiag );
			parameters.add( cuT( "cornerRounding" ), context.cornerRounding );
		}

		if ( context.mode == Mode::eT2X )
		{
			parameters.add( cuT( "reprojection" ), context.reprojection );
			parameters.add( cuT( "reprojectionWeightScale" ), context.reprojectionWeightScale );
		}

		auto effect = engine->getRenderTargetCache().getPostEffectFactory().create( PostEffect::Type
			, *parsingContext->renderTarget
			, *engine->getRenderSystem()
			, parameters );
		parsingContext->renderTarget->addPostEffect( effect );

		delete reinterpret_cast< ParserContext * >( p_context->unregisterUserContext( PostEffect::Type ) );
	}
	END_ATTRIBUTE_POP()
}
