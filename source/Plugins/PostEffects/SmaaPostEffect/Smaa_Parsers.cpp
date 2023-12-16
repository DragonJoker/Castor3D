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
	CU_ImplementAttributeParserNewBlock( parserSmaa, castor3d::TargetContext, SmaaContext )
	{
		newBlockContext->renderTarget = blockContext->renderTarget;
	}
	CU_EndAttributePushNewBlock( SmaaSection::eRoot )

	CU_ImplementAttributeParserBlock( parserMode, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->data.mode = Mode( params[0]->get< uint32_t >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserPreset, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			blockContext->preset = Preset( params[0]->get< uint32_t >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserEdgeDetection, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			blockContext->data.edgeDetection = EdgeDetectionType( params[0]->get< uint32_t >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDisableDiagonalDetection, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.disableDiagonalDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDisableCornerDetection, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.disableCornerDetection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserThreshold, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.threshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMaxSearchSteps, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.maxSearchSteps );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMaxSearchStepsDiag, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.maxSearchStepsDiag );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCornerRounding, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.cornerRounding );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParserBlock( parserPredication, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.enablePredication );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParserBlock( parserReprojection, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.enableReprojection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserReprojectionWeightScale, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.reprojectionWeightScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserLocalContrastAdaptationFactor, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.localContrastAdaptationFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserPredicationScale, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.predicationScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserPredicationStrength, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.predicationStrength );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserPredicationThreshold, SmaaContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.predicationThreshold );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSmaaEnd, SmaaContext )
	{
		castor3d::Parameters parameters;
		parameters.add( cuT( "mode" ), getName( blockContext->data.mode ) );
		parameters.add( cuT( "preset" ), getName( blockContext->preset ) );
		parameters.add( cuT( "edgeDetection" ), getName( blockContext->data.edgeDetection ) );
		parameters.add( cuT( "disableDiagonalDetection" ), blockContext->data.disableDiagonalDetection );
		parameters.add( cuT( "disableCornerDetection" ), blockContext->data.disableCornerDetection );
		parameters.add( cuT( "localContrastAdaptationFactor" ), blockContext->data.localContrastAdaptationFactor );
		parameters.add( cuT( "enablePredication" ), blockContext->data.enablePredication );
		parameters.add( cuT( "predicationScale" ), blockContext->data.predicationScale );
		parameters.add( cuT( "predicationStrength" ), blockContext->data.predicationStrength );
		parameters.add( cuT( "predicationThreshold" ), blockContext->data.predicationThreshold );

		if ( blockContext->preset == Preset::eCustom )
		{
			parameters.add( cuT( "threshold" ), blockContext->data.threshold );
			parameters.add( cuT( "maxSearchSteps" ), blockContext->data.maxSearchSteps );
			parameters.add( cuT( "maxSearchStepsDiag" ), blockContext->data.maxSearchStepsDiag );
			parameters.add( cuT( "cornerRounding" ), blockContext->data.cornerRounding );
		}

		if ( blockContext->data.mode == Mode::eT2X )
		{
			parameters.add( cuT( "enableReprojection" ), blockContext->data.enableReprojection );
			parameters.add( cuT( "reprojectionWeightScale" ), blockContext->data.reprojectionWeightScale );
		}

		auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );
	}
	CU_EndAttributePop()
}
