#include "SmaaPostEffect/SmaaPostEffect.hpp"
#include "SmaaPostEffect/Smaa_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Plugin/PostFxPlugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Math/Range.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Smaa_API
#else
#	ifdef SmaaPostEffect_EXPORTS
#		define C3D_Smaa_API __declspec( dllexport )
#	else
#		define C3D_Smaa_API __declspec( dllimport )
#	endif
#endif

namespace
{
	castor::AttributeParsers createParsers()
	{
		static castor::UInt32StrMap modes
		{
			{ "1X", uint32_t( smaa::Mode::e1X ) },
			{ "T2X", uint32_t( smaa::Mode::eT2X ) },
			{ "S2X", uint32_t( smaa::Mode::eS2X ) },
			{ "4X", uint32_t( smaa::Mode::e4X ) }
		};
		static castor::UInt32StrMap presets
		{
			{ "low", uint32_t( smaa::Preset::eLow ) },
			{ "medium", uint32_t( smaa::Preset::eMedium ) },
			{ "high", uint32_t( smaa::Preset::eHigh ) },
			{ "ultra", uint32_t( smaa::Preset::eUltra ) },
			{ "custom", uint32_t( smaa::Preset::eCustom ) }
		};
		static castor::UInt32StrMap detections
		{
			{ "depth", uint32_t( smaa::EdgeDetectionType::eDepth ) },
			{ "colour", uint32_t( smaa::EdgeDetectionType::eColour ) },
			{ "luma", uint32_t( smaa::EdgeDetectionType::eLuma ) }
		};
		castor::AttributeParsers result;

		addParser( result, uint32_t( castor3d::CSCNSection::eRenderTarget ), cuT( "smaa" ), &smaa::parserSmaa );

		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "mode" ), &smaa::parserMode, { castor::makeParameter< castor::ParameterType::eCheckedText >( "SMAAMode", modes ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "preset" ), &smaa::parserPreset, { castor::makeParameter< castor::ParameterType::eCheckedText >( "SMAAPreset", presets ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "edgeDetection" ), &smaa::parserEdgeDetection, { castor::makeParameter< castor::ParameterType::eCheckedText >( "SMAADepthDetection", detections ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "disableDiagonalDetection" ), &smaa::parserDisableDiagonalDetection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "disableCornerDetection" ), &smaa::parserDisableCornerDetection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "threshold" ), &smaa::parserThreshold, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 0.5f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchSteps" ), &smaa::parserMaxSearchSteps, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 112 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "maxSearchStepsDiag" ), &smaa::parserMaxSearchStepsDiag, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 20 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "cornerRounding" ), &smaa::parserCornerRounding, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 100 ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "enablePredication" ), &smaa::parserPredication, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "enableReprojection" ), &smaa::parserReprojection, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationScale" ), &smaa::parserPredicationScale, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 1.0f, 5.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationStrength" ), &smaa::parserPredicationStrength, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 1.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "reprojectionWeightScale" ), &smaa::parserReprojectionWeightScale, { castor::makeParameter< castor::ParameterType::eFloat >( castor::makeRange( 0.0f, 80.0f ) ) } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "localContrastAdaptationFactor" ), &smaa::parserLocalContrastAdaptationFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "predicationThreshold" ), &smaa::parserPredicationThreshold, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( result, uint32_t( smaa::SmaaSection::eRoot ), cuT( "}" ), &smaa::parserSmaaEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( smaa::SmaaSection::eRoot ), cuT( "smaa" ) },
		};
	}

	void * createContext( castor::FileParserContext & context )
	{
		smaa::ParserContext * smaaContext = new smaa::ParserContext;
		smaaContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		return smaaContext;
	}
}

extern "C"
{
	C3D_Smaa_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_Smaa_API void getType( castor3d::PluginType * p_type );
	C3D_Smaa_API void getName( char const ** p_name );
	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_Smaa_API void OnUnload( castor3d::Engine * engine );

	C3D_Smaa_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_Smaa_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::ePostEffect;
	}

	C3D_Smaa_API void getName( char const ** name )
	{
		*name = smaa::PostEffect::Name.c_str();
	}

	C3D_Smaa_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->getPostEffectFactory().registerType( smaa::PostEffect::Type
			, &smaa::PostEffect::create );
		engine->registerParsers( smaa::PostEffect::Type
			, createParsers()
			, createSections()
			, &createContext );
	}

	C3D_Smaa_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( smaa::PostEffect::Type );
		engine->getPostEffectFactory().unregisterType( smaa::PostEffect::Type );
	}
}
