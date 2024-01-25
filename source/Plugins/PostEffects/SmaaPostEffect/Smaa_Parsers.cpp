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
	namespace parse
	{
		struct SmaaContext
		{
			castor3d::RenderTargetRPtr renderTarget{};
			Preset preset{};
			SmaaConfig::Data data{};
		};

		enum class SmaaSection
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'S', 'M', 'A', 'A' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserSmaa, castor3d::TargetContext, SmaaContext )
		{
			newBlockContext->renderTarget = blockContext->renderTarget;
		}
		CU_EndAttributePushNewBlock( SmaaSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserMode, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->data.mode = Mode( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPreset, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				uint32_t value{ 0u };
				params[0]->get( value );
				blockContext->preset = Preset( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEdgeDetection, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				uint32_t value{ 0u };
				params[0]->get( value );
				blockContext->data.edgeDetection = EdgeDetectionType( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDisableDiagonalDetection, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.disableDiagonalDetection );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDisableCornerDetection, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.disableCornerDetection );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserThreshold, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.threshold );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxSearchSteps, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.maxSearchSteps );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxSearchStepsDiag, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.maxSearchStepsDiag );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCornerRounding, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.cornerRounding );
			}
		}
		CU_EndAttribute()
		
		static CU_ImplementAttributeParserBlock( parserPredication, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.enablePredication );
			}
		}
		CU_EndAttribute()
		
		static CU_ImplementAttributeParserBlock( parserReprojection, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.enableReprojection );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserReprojectionWeightScale, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.reprojectionWeightScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLocalContrastAdaptationFactor, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.localContrastAdaptationFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPredicationScale, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.predicationScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPredicationStrength, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.predicationStrength );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPredicationThreshold, SmaaContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->data.predicationThreshold );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSmaaEnd, SmaaContext )
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

	castor::AttributeParsers createParsers()
	{
		static castor::UInt32StrMap modes
		{
			{ cuT( "1X" ), uint32_t( smaa::Mode::e1X ) },
			{ cuT( "T2X" ), uint32_t( smaa::Mode::eT2X ) },
			{ cuT( "S2X" ), uint32_t( smaa::Mode::eS2X ) },
			{ cuT( "4X" ), uint32_t( smaa::Mode::e4X ) }
		};
		static castor::UInt32StrMap presets
		{
			{ cuT( "low" ), uint32_t( smaa::Preset::eLow ) },
			{ cuT( "medium" ), uint32_t( smaa::Preset::eMedium ) },
			{ cuT( "high" ), uint32_t( smaa::Preset::eHigh ) },
			{ cuT( "ultra" ), uint32_t( smaa::Preset::eUltra ) },
			{ cuT( "custom" ), uint32_t( smaa::Preset::eCustom ) }
		};
		static castor::UInt32StrMap detections
		{
			{ cuT( "depth" ), uint32_t( smaa::EdgeDetectionType::eDepth ) },
			{ cuT( "colour" ), uint32_t( smaa::EdgeDetectionType::eColour ) },
			{ cuT( "luma" ), uint32_t( smaa::EdgeDetectionType::eLuma ) }
		};
		castor::AttributeParsers result;

		addParserT( result
			, castor3d::CSCNSection::eRenderTarget
			, parse::SmaaSection::eRoot
			, cuT( "smaa" )
			, &parse::parserSmaa );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "mode" )
			, &parse::parserMode
			, { castor::makeParameter< castor::ParameterType::eCheckedText >( cuT( "SMAAMode" ), modes ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "preset" )
			, &parse::parserPreset
			, { castor::makeParameter< castor::ParameterType::eCheckedText >( cuT( "SMAAPreset" ), presets ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "edgeDetection" )
			, &parse::parserEdgeDetection
			, { castor::makeParameter< castor::ParameterType::eCheckedText >( cuT( "SMAADepthDetection" ), detections ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "disableDiagonalDetection" )
			, &parse::parserDisableDiagonalDetection
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "disableCornerDetection" )
			, &parse::parserDisableCornerDetection
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "threshold" )
			, &parse::parserThreshold
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 0.5f ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "maxSearchSteps" )
			, &parse::parserMaxSearchSteps
			, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 112 ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "maxSearchStepsDiag" )
			, &parse::parserMaxSearchStepsDiag
			, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 20 ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "cornerRounding" )
			, &parse::parserCornerRounding
			, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 100 ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "enablePredication" )
			, &parse::parserPredication
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "enableReprojection" )
			, &parse::parserReprojection
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "predicationScale" )
			, &parse::parserPredicationScale
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 1.0f, 5.0f ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "predicationStrength" )
			, &parse::parserPredicationStrength
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "reprojectionWeightScale" )
			, &parse::parserReprojectionWeightScale
			, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 80.0f ) ) } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "localContrastAdaptationFactor" )
			, &parse::parserLocalContrastAdaptationFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, cuT( "predicationThreshold" )
			, &parse::parserPredicationThreshold
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::SmaaSection::eRoot
			, castor3d::CSCNSection::eRenderTarget
			, cuT( "}" )
			, &parse::parserSmaaEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( parse::SmaaSection::eRoot ), cuT( "smaa" ) },
		};
	}
}
